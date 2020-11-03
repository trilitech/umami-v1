module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: option(Account.t),
    recipient: option(Account.t),
    fee: string,
    counter: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
  }
];

include ReForm.Make(StateLenses);
