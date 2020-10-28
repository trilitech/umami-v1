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
