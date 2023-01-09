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
    "alias": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
    "balanceEmpty": style(~height=4.->dp, ()),
    "address": style(~height=18.->dp, ()),
    "description": style(~height=18.->dp, ()),
  })
}

module GenericAccountInfo = {
  @react.component
  let make = (
    ~name: string,
    ~address:PublicKeyHash.t,
    ~token: option<Token.t>=?,
    ~showBalance=true,
    ~showAlias=true,
    ~forceFetch,
    ~description:option<React.element>=?
  ) =>
    <View>
      {<Typography.Subtitle1 style={styles["alias"]}>
        {name->React.string}
      </Typography.Subtitle1>->ReactUtils.onlyWhen(showAlias)}
      {showBalance
        ? <AccountInfoBalance forceFetch address ?token />
        : <View style={styles["balanceEmpty"]} />}
      {Option.getWithDefault(description, React.null)}
      <Typography.Address style={styles["address"]}>
        {(address :> string)->React.string}
      </Typography.Address>
    </View>
}
