/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: option(Account.t),
    recipient: FormUtils.Alias.any,
    fee: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
    dryRun: option(Protocol.simulationResults),
  }
];

type validState = {
  amount: Transfer.currency,
  sender: Account.t,
  recipient: FormUtils.Alias.t,
  fee: option(Tez.t),
  gasLimit: option(int),
  storageLimit: option(int),
  forceLowFee: bool,
  dryRun: option(Protocol.simulationResults),
};

let unsafeExtractValidState = (token, state: StateLenses.state): validState => {
  {
    amount:
      state.amount
      ->FormUtils.parseAmount(token)
      ->FormUtils.Unsafe.getCurrency,
    sender: state.sender->FormUtils.Unsafe.getValue,
    recipient: state.recipient->FormUtils.Unsafe.account,
    fee: state.fee->Tez.fromString,
    gasLimit: state.gasLimit->Int.fromString,
    storageLimit: state.storageLimit->Int.fromString,
    forceLowFee: state.forceLowFee,
    dryRun: state.dryRun,
  };
};

let toState = (vs: validState): StateLenses.state => {
  amount: vs.amount->Transfer.currencyToString,
  sender: vs.sender->Some,
  recipient: vs.recipient->FormUtils.Alias.Valid,

  fee: vs.fee->Option.mapWithDefault("", Tez.toString),
  gasLimit: vs.gasLimit->FormUtils.optToString(Int.toString),
  storageLimit: vs.storageLimit->FormUtils.optToString(Int.toString),
  forceLowFee: vs.forceLowFee,
  dryRun: vs.dryRun,
};

include ReForm.Make(StateLenses);

module Password = {
  module StateLenses = [%lenses type state = {password: string}];

  include ReForm.Make(StateLenses);
};

type transaction = Transfer.t;

let buildTransferElts = (transfers, build) => {
  transfers->List.map(((t: validState, advOpened)) => {
    let destination = t.recipient->FormUtils.Alias.address;

    let gasLimit = advOpened ? t.gasLimit : None;
    let storageLimit = advOpened ? t.storageLimit : None;
    let fee = advOpened ? t.fee : None;

    build(
      ~destination,
      ~amount=t.amount,
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    );
  });
};

let buildTransfer = (inputTransfers, source, forceLowFee) =>
  Transfer.makeTransfers(
    ~source=source.Account.address,
    ~transfers=
      buildTransferElts(
        inputTransfers,
        Transfer.makeSingleTransferElt(~parameter=?None, ~entrypoint=?None),
      ),
    ~forceLowFee?,
    (),
  );

let buildTransaction = (batch: list((validState, bool))) => {
  switch (batch) {
  | [] => assert(false)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    buildTransfer(inputTransfers, source, forceLowFee);
  };
};
