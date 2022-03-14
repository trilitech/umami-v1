open Protocol;
open Protocol.Transfer;
open SendForm;

let getTokenId = (t: TokenRepr.t) => TokenRepr.kindId(t.kind);

let tokenIsFa2 = (t: TokenRepr.t) =>
  switch (t.kind) {
  | FA1_2 => false
  | FA2(_) => true
  };

let makeTransferFA2 =
    (token: ProtocolAmount.token, recipient: FormUtils.Alias.t) => {
  let newTransferFA2: Protocol.Transfer.transferFA2 = {
    tokenId: token.token->getTokenId,
    content: {
      amount: token,
      destination: recipient->FormUtils.Alias.address,
    },
  };
  newTransferFA2;
};

let makeFA2Data = (token, recipient) => {
  let transferFa2 = makeTransferFA2(token, recipient);
  let batchFA2: Protocol.Transfer.batchFA2 = {
    address: token.token.address,
    transfers: [transferFa2],
  };
  FA2Batch(batchFA2);
};

let validStateToSimpleData = (validState: SendForm.validState) => {
  let result: Protocol.Transfer.data =
    Simple({
      amount: validState.amount,
      destination: validState.recipient->FormUtils.Alias.address,
    });
  result;
};

let validStateToTransferData = (validState: SendForm.validState) => {
  switch (validState.amount) {
  | Tez(_) => validStateToSimpleData(validState)
  | Token(token) =>
    if (tokenIsFa2(token.token)) {
      makeFA2Data(token, validState.recipient);
    } else {
      validStateToSimpleData(validState);
    }
  };
};

// Adds default options to Trasfer.data to create a Transfer.t
let transferDataToTransfer = (data: Transfer.data, parameter, entrypoint) => {
  let parameter =
    ProtocolOptions.makeParameter(~value=?parameter, ~entrypoint?, ());
  let options = ProtocolOptions.make();
  {data, parameter, options};
};

let validStateToTransfer = x =>
  x
  ->validStateToTransferData
  ->transferDataToTransfer(x.parameter, x.entrypoint);

let validStateToManager = x => x->validStateToTransfer->Transfer;

// Append or merge to element in managers
let addTransferDataToBatch = (_data, _batch) => ();
