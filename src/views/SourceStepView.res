/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */
open ReactNative

type state = (PublicKeyHash.t, array<Protocol.manager>, option<Alias.t>) // ((initiator, managers), source)
type stack = list<state>

let zeroTez = Protocol.Amount.Tez(Tez.zero)

let back = (~default=?, (stack, setStack)) =>
  switch stack {
  | list{_} | list{} => default
  | list{_, ...tl} => Some(_ => setStack(_ => tl))
  }

let proposal = (destination, managers) => {
  let entrypoint = "propose"
  let parameter = ProtocolHelper.Multisig.batch(managers)
  [ProtocolHelper.Multisig.call(~entrypoint, ~parameter, destination)]
}

// Design choice: you need to create the stack state in the parent container
// This will allow you to properly handle the back button behavior of your form
// containing the SourceStepView component. e.g.
//
//  let back =
//    switch modalStep {
//    | SourceStep =>
//      let default = () => setModalStep(_ => SendStep)
//      SourceStepView.back(~default, stackState)
//    [...]
//    }
@ocaml.doc("Stack should be initialized with one the operation you want to execute,
            and it will automatically be wrapped into a proposal as many time as needed.")
@react.component
let make = (~stack, ~callback) => {
  let (stack, setStack) = stack

  @warning("-8")
  let list{(initiator, managers, source), ...stack} = stack
  // If stack has only one element, it means that it does not have been wrapped yet.
  // Otherwise, source step already wrapped it and we can submit it as is.
  let managers = list{} == stack ? proposal(initiator, managers) : managers

  @warning("+8")
  let accounts = StoreContext.AccountsMultisigs.useRequest()->ApiRequest.getWithDefault(
    PublicKeyHash.Map.empty,
  )
  let multisigFromAddress = StoreContext.Multisig.useGetFromAddress()
  let multisig = multisigFromAddress(initiator)->Option.getExn
  let filter = Js.Array2.includes(multisig.Multisig.signers)
  switch switch source {
  | Some(_) => source
  | None => PublicKeyHash.Map.findFirstBy(accounts, (k, _) => filter(k))->Option.map(snd)
  } {
  | Some(source) =>
    let onSubmit = (submitter: Alias.t) => {
      submitter.kind == Some(Multisig)
        ? {
            let stack = list{(initiator, managers, submitter->Some), ...stack}
            let managers = proposal(submitter.address, managers)
            let stack = list{(submitter.address, managers, None), ...stack}
            setStack(_ => stack)
          }
        : {
            let account = Alias.toAccountExn(submitter)
            callback(account, managers)
          }
    }
    // Key is needed in order to reset the 'selectedAccount' state in SourceSelector...
    let key = "SourceStepView" ++ (multisig.address :> string)
    <SourceSelector multisig=multisig.address source filter onSubmit key />
  | None => <View> {I18n.Form_input_error.permissions_error->Typography.body1} </View>
  }
}
