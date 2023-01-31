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
open Style

let styles = {
  open Style
  StyleSheet.create({
    "alias": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
    "balanceEmpty": style(~height=4.->dp, ()),
    "address": style(~height=18.->dp, ()),
    "description": style(~height=18.->dp, ()),
    "button": style(~marginRight=4.->dp, ()),
    "actionButtons": style(~alignSelf=#flexEnd, ~flexDirection=#row, ~flex=1., ()),
  })
}

module AccountAdressActionButtons = {
  @react.component
  let make = (~account: Alias.t) => {
    let addToast = LogsContext.useToast()

    <View style={styles["actionButtons"]}>
      <ClipboardButton
        copied=I18n.Log.address
        tooltipKey={(account.address :> string)}
        addToast
        data={(account.address :> string)}
        style={styles["button"]}
      />
      <QrButton tooltipKey={(account.address :> string)} account style={styles["button"]} />
    </View>
  }
}
module GenericAccountInfo = {
  @react.component
  let make = (
    ~token: option<Token.t>=?,
    ~showBalance=true,
    ~showAlias=true,
    ~forceFetch,
    ~description: option<React.element>=?,
    ~account: Alias.t,
  ) =>
    <View>
      {<Typography.Subtitle1 style={styles["alias"]}>
        {account.name->React.string}
      </Typography.Subtitle1>->ReactUtils.onlyWhen(showAlias)}
      {showBalance
        ? <AccountInfoBalance forceFetch address=account.address ?token />
        : <View style={styles["balanceEmpty"]} />}
      {Option.getWithDefault(description, React.null)}
      <View style={style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ())}>
        <Typography.Address style={styles["address"]}>
          {(account.address :> string)->React.string}
        </Typography.Address>
        <AccountAdressActionButtons account />
      </View>
    </View>
}
