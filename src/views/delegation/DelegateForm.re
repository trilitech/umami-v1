module StateLenses = [%lenses
  type state = {
    sender: string,
    baker: string,
    fee: string,
    burnCap: string,
    forceLowFee: bool,
  }
];

include ReForm.Make(StateLenses);

module Password = {
  module StateLenses = [%lenses type state = {password: string}];

  include ReForm.Make(StateLenses);
};
