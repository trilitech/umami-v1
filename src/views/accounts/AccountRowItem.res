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

let styles = {
  open Style
  StyleSheet.create({
    "inner": style(~marginRight=10.->dp, ~flexDirection=#row, ~alignItems=#flexStart, ()),
    "actionContainer": style(~marginRight=24.->dp, ()),
  })
}

module GenericRowItem = {
  @react.component
  let make = (
    ~account: Alias.t,
    ~token: option<Token.t>=?,
    ~showBalance=?,
    ~showAlias=?,
    ~forceFetch=false,
    ~icon_isHD=false,
    ~description: option<React.element>=?,
    ~children: React.element,
  ) => {
    open Style
    <RowItem.Bordered
      minHeight=90.
      innerStyle={style(~alignSelf=#flexStart, ~marginTop=8.->dp, ~paddingVertical=8.->dp, ())}>
      <View style={styles["inner"]}>
        <AliasIcon style={SecretRowTree.styles["iconContainer"]} kind=account.kind isHD=icon_isHD />
        <AccountInfo.GenericAccountInfo
          ?token ?showBalance ?showAlias ?description forceFetch account
        />
      </View>
      children
    </RowItem.Bordered>
  }
}

module MultisigRowItem = {
  @react.component
  let make = (~multisig: Multisig.t) => {
    let description = {
      let threshold = multisig.threshold->ReBigNumber.toString
      let signers = Array.length(multisig.signers)->Int.toString
      <Typography.Body2 style={AccountInfo.styles["description"]}>
        {I18n.Label.approval_threshold->React.string}
        {" "->React.string}
        {I18n.Label.out_of(threshold, signers)->React.string}
      </Typography.Body2>
    }
    <GenericRowItem account={multisig->Alias.fromMultisig} description>
      {React.null}
    </GenericRowItem>
  }
}

@react.component
let make = (~account: Account.t, ~isHD: bool=false, ~token: option<Token.t>=?) => {
  let delegateRequest = StoreContext.Delegate.useLoad(account.address)
  let balanceRequest =
    StoreContext.Balance.useAll(false)->StoreContext.Balance.useOne(account.address)
  let zeroTez = switch balanceRequest->ApiRequest.getDoneOk {
  | None => true
  | Some(balance) => balance == Tez.zero
  }
  <GenericRowItem account={account->Alias.fromAccount} ?token icon_isHD=isHD>
    {delegateRequest
    ->ApiRequest.mapWithDefault(React.null, delegate =>
      <View style={styles["actionContainer"]}>
        <DelegateButton
          zeroTez
          action={delegate->Option.mapWithDefault(
            Delegate.Create(account, false),
            delegate => Delegate.Edit(account, delegate),
          )}
        />
      </View>
    )
    ->ReactUtils.onlyWhen(token == None)}
  </GenericRowItem>
}
