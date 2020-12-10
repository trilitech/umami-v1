module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: string,
    recipient: string,
    fee: string,
    counter: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
  }
];

include ReForm.Make(StateLenses);

module Password = {
  module StateLenses = [%lenses type state = {password: string}];

  include ReForm.Make(StateLenses);
};

type operation =
  | InjectionOperation(Injection.operation)
  | TokensOperation(Tokens.operation, Token.t);
