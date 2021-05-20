type strictAmount =
  | XTZ(ProtocolXTZ.t)
  | Token(Token.Unit.t, Token.t);

type amount =
  | Amount(strictAmount)
  | Illformed(string);

let xtzAmount = v => v->XTZ->Amount;
let tkAmount = (v, tk) => Token(v, tk)->Amount;

let parseAmount = (v, token) =>
  if (v == "") {
    None;
  } else {
    token->Option.mapWithDefault(
      {
        let vxtz = v->ProtocolXTZ.fromString;
        vxtz == None ? v->Illformed->Some : vxtz->Option.map(xtzAmount);
      },
      t => {
        let vt = v->Token.Unit.fromNatString;
        vt == None ? v->Illformed->Some : vt->Option.map(v => v->tkAmount(t));
      },
    );
  };

let fromTransferCurrency = (~token=?, amount) =>
  switch (amount, token) {
  | (Transfer.XTZ(v), None) => XTZ(v)
  | (Token((v, _)), Some(t)) => Token(v, t)
  /* This function should actually be removed later */
  | (_, _) => assert(false)
  };

let amountToString =
  fun
  | Token(v, _) => v->Token.Unit.toNatString
  | XTZ(v) => v->ProtocolXTZ.toString;

let keepToken = v =>
  v->Option.flatMap(
    fun
    | Illformed(_)
    | Amount(XTZ(_)) => None
    | Amount(Token(v, _)) => v->Some,
  );

let keepXTZ = v =>
  v->Option.flatMap(
    fun
    | Amount(XTZ(v)) => v->Some
    | Illformed(_)
    | Amount(Token(_)) => None,
  );

let keepStrictToken =
  fun
  | XTZ(_) => None
  | Token(v, t) => Some((v, t));

let keepStrictXTZ =
  fun
  | XTZ(v) => v->Some
  | Token(_) => None;

let optToString = (v, f) => v->Option.mapWithDefault("", f);

module Account = {
  type t =
    | Address(string)
    | Account(Account.t);

  type any =
    | AnyString(string)
    | Valid(t);

  let address =
    fun
    | Address(s) => s
    | Account(a) => a.address;

  let alias =
    fun
    | Address(_) => ""
    | Account(a) => a.alias;
};

module Unsafe = {
  // more explicit than assert(false)

  let getValue =
    fun
    | None => failwith("Should not be None")
    | Some(v) => v;

  let getCurrency = v =>
    switch (v) {
    | None => failwith("Should not be empty")
    | Some(Illformed(_)) => failwith("Should not be malformed")
    | Some(Amount(a)) => a
    };

  let getXTZ = v =>
    switch (v) {
    | Amount(XTZ(a)) => a
    | Illformed(_)
    | Amount(Token(_)) => failwith("Should not be malformed")
    };

  let account =
    fun
    | Account.AnyString(_) => failwith("Should be an address or an alias")
    | Account.Valid(a) => a;
};

let emptyOr = (f, v): ReSchema.fieldState => v == "" ? Valid : f(v);

let isValidXtzAmount: string => ReSchema.fieldState =
  fun
  | s when ProtocolXTZ.fromString(s) != None => Valid
  | "" => Error(I18n.form_input_error#mandatory)
  | _ => Error(I18n.form_input_error#float);

let isValidTokenAmount: string => ReSchema.fieldState =
  fun
  | s when Token.Unit.forceFromString(s) != None => Valid
  | "" => Error(I18n.form_input_error#mandatory)
  | _ => Error(I18n.form_input_error#int);

let notNone = (v): ReSchema.fieldState =>
  v != None ? Valid : Error(I18n.form_input_error#mandatory);

let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN
      ? Error(I18n.form_input_error#float) : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->int_of_string_opt->Option.isSome
      ? Valid : Error(I18n.form_input_error#int);
  fieldState;
};

let formFieldsAreValids = (fieldsState, validateFields) => {
  let fields = fieldsState->Array.map(((field, _)) => field);
  let fieldsState = validateFields(fields);

  fieldsState->Array.every((fieldState: ReForm.fieldState) => {
    switch (fieldState) {
    | Valid => true
    | _ => false
    }
  });
};

let i18n = {
  ...ReSchemaI18n.default,
  stringNonEmpty: (~value as _) => I18n.form_input_error#mandatory,
};
