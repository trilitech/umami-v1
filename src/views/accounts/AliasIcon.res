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
  StyleSheet.create({"tag": style(~width=40.->dp, ~height=18.->dp, ())})
}

@react.component
let make = (~style=?, ~kind: option<Alias.kind>, ~isHD: bool) => {
  let theme = ThemeContext.useTheme()
  let icon = (builder: Icons.builder) =>
    builder(~style=?None, ~size=20., ~color=theme.colors.iconMediumEmphasis)

  <View ?style>
    {switch kind {
    | Some(Account(Encrypted | Unencrypted)) if !isHD =>
      <Tag style={styles["tag"]} content=I18n.Label.account_umami />
    | Some(Account(Encrypted | Unencrypted)) =>
      icon(Icons.Umami.build)
    | Some(Account(Galleon)) =>
      icon(Icons.Galleon.build)
    | Some(Account(Ledger)) =>
      icon(Icons.LedgerP.build)
    | Some(Account(CustomAuth(infos))) =>
      icon(infos.provider->CustomAuthProviders.getIcon)
    | None =>
      React.null
    | Some(Contact) =>
      Icons.AddressBook.build(~style=?None, ~size=20., ~color=theme.colors.iconMediumEmphasis)
    | Some(Multisig) =>
      icon(Icons.Key.build)
    }}
  </View>
}
