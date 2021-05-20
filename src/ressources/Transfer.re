open ProtocolOptions;

type currency =
  | XTZ(ProtocolXTZ.t)
  | Token(TokenRepr.currency);

type elt = {
  destination: string,
  amount: currency,
  tx_options: transferOptions,
};

type t = {
  source: string,
  transfers: list(elt),
  common_options: commonOptions,
};

let makeXTZ = t => t->XTZ;
let makeToken = (~amount, ~token) => Token((amount, token));

let currencyToInt64 =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toInt64
  | Token((curr, _)) => curr->TokenRepr.Unit.toBigNumber->ReBigNumber.toInt64;

let currencyToBigNumber =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toInt64->ReBigNumber.fromInt64
  | Token((curr, _)) => curr->TokenRepr.Unit.toBigNumber;

let currencyToString =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toString
  | Token((curr, _)) => curr->TokenRepr.Unit.toNatString;

let getXTZ =
  fun
  | XTZ(xtz) => Some(xtz)
  | _ => None;

let getToken =
  fun
  | Token(t) => Some(t)
  | _ => None;

let getTokenExn = t => t->getToken->Option.getExn;

let makeSingleTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  destination,
  amount,
  tx_options:
    makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter,
      ~entrypoint,
      (),
    ),
};

let makeSingleXTZTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=makeXTZ(amount),
    ~fee?,
    ~parameter?,
    ~entrypoint?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

/* Tokens cannot define parameter and entrypoint, since they are
   already translated as parameters into an entrypoint */
let makeSingleTokenTransferElt =
    (~destination, ~amount, ~token, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=makeToken(~amount, ~token),
    ~fee?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

let makeTransfers =
    (~source, ~transfers, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  let common_options = makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ());
  {source, transfers, common_options};
};
