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
  }
];

type validState = {
  amount: Transfer.Currency.t,
  sender: Account.t,
  recipient: FormUtils.Alias.t,
};

let unsafeExtractValidState = (token, state: StateLenses.state): validState => {
  {
    amount:
      state.amount
      ->FormUtils.parseAmount(token)
      ->FormUtils.Unsafe.getCurrency,
    sender: state.sender->FormUtils.Unsafe.getValue,
    recipient: state.recipient->FormUtils.Unsafe.account,
  };
};

let toState = (vs: validState): StateLenses.state => {
  amount: vs.amount->Transfer.Currency.toString,
  sender: vs.sender->Some,
  recipient: vs.recipient->FormUtils.Alias.Valid,
};

include ReForm.Make(StateLenses);

let buildTransferElts = (transfers, build) => {
  transfers->List.map((t: validState) => {
    let destination = t.recipient->FormUtils.Alias.address;

    build(~destination, ~amount=t.amount, ());
  });
};

let buildTransfer = (inputTransfers, source) =>
  Transfer.makeTransfers(
    ~source,
    ~transfers=
      buildTransferElts(inputTransfers, (~destination, ~amount, ()) =>
        Transfer.makeSingleTransferElt(~destination, ~amount, ())
      ),
    (),
  );

let buildTransaction = (batch: list(validState)) => {
  switch (batch) {
  | [] => assert(false)
  | [first, ..._] as inputTransfers =>
    let source = first.sender;

    buildTransfer(inputTransfers, source);
  };
};
