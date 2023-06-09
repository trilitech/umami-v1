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

let controlView = {
  open Style
  array([NftRowToken.styles["flex_row"], style(~borderTopWidth=1., ())])
}

let styles = {
  open Style
  StyleSheet.create({
    "ctrlButton": style(~marginRight=-5.->dp, ()),
    "thin": style(~maxHeight=27.->dp, ~minHeight=27.->dp, ~justifyContent=#center, ()),
  })
}

@react.component
let make = (~nfts: TokensLibrary.WithBalance.t, ~account) => {
  let (selected, setSelected) = React.useState(_ => PublicKeyHash.Map.empty)

  let setSelectedToken = (pkh, id, checked) =>
    setSelected(map => map->NftSelection.updateSelection(pkh, id, checked))

  let (hidden, setHidden) = // here we load, and at each set, we write.
  React.useState(_ =>
    switch TokenStorage.Registered.get() {
    | Ok(map) => map
    | Error(_) => PublicKeyHash.Map.empty
    }
  )

  let setHidden = (selected, hidden) =>
    setHidden(_ =>
      switch TokensAPI.updateNFTsVisibility(selected, ~hidden) {
      | Ok(r) => r
      | Error(_) => PublicKeyHash.Map.empty
      }
    )

  let theme = ThemeContext.useTheme()

  let allTokensId = React.useMemo1(
    () => nfts->PublicKeyHash.Map.map(c => c.TokensLibrary.Generic.tokens->Map.Int.map(_ => ())),
    [nfts],
  )

  let checked = React.useMemo2(
    () =>
      !(allTokensId->NftSelection.isEmpty) &&
      allTokensId->NftSelection.size == selected->NftSelection.size,
    (selected, allTokensId),
  )

  let (oneSelectedHidden, oneSelectedShown) = React.useMemo2(() =>
    selected->PublicKeyHash.Map.reduce((false, false), ((oneHidden, oneShown), address, ids) =>
      ids->Map.Int.reduce((oneHidden, oneShown), ((oneHidden, oneShown), id, _) =>
        oneHidden && oneShown
          ? (oneHidden, oneShown) // avoids calling isHidden if not necessary
          : {
              let currentHidden = RegisteredTokens.isHidden(hidden, address, id)
              (oneHidden || currentHidden, oneShown || !currentHidden)
            }
      )
    )
  , (selected, hidden))

  let contracts =
    nfts->PublicKeyHash.Map.map(contract =>
      <NftRowContract contract account selected setSelected=setSelectedToken hidden setHidden />
    )
    |> PublicKeyHash.Map.valuesToArray
    |> React.array

  nfts->PublicKeyHash.Map.isEmpty
    ? <NftEmptyView />
    : <>
        <View
          style={
            open Style
            array([controlView, style(~borderColor=theme.colors.borderDisabled, ())])
          }>
          <CheckboxItem
            style={
              open Style
              array([
                NftRowToken.styles["checkboxMargin"],
                style(~tintColor=theme.colors.iconPrimary, ()),
              ])
            }
            value=checked
            handleChange={checked =>
              checked ? setSelected(_ => allTokensId) : setSelected(_ => PublicKeyHash.Map.empty)}
          />
          {ReactUtils.onlyWhen(
            <IconButton
              iconSizeRatio={5. /. 7.}
              style={
                open Style
                array([NftRowToken.styles["marginLeft10"], styles["ctrlButton"]])
              }
              icon=Icons.Eye.build
              onPress={_ => setHidden(selected, false)}
            />,
            oneSelectedHidden,
          )}
          {ReactUtils.onlyWhen(
            <IconButton
              style={
                open Style
                array([NftRowToken.styles["marginLeft10"], styles["ctrlButton"]])
              }
              iconSizeRatio={5. /. 7.}
              icon=Icons.EyeStrike.build
              onPress={_ => setHidden(selected, true)}
            />,
            oneSelectedShown,
          )}
        </View>
        <ScrollView> contracts </ScrollView>
      </>
}
