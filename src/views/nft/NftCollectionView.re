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

open ReactNative;

let controlView =
  Style.(
    array([|
      NftRowToken.styles##flex_row,
      style(~marginTop=16.->dp, ~borderTopWidth=1., ()),
    |])
  );

let styles =
  Style.(
    StyleSheet.create({
      "ctrlButton": style(~marginRight=(-5.)->dp, ()),
      "thin":
        style(
          ~maxHeight=27.->dp,
          ~minHeight=27.->dp,
          ~justifyContent=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~nfts: TokenRegistry.Cache.t, ~account) => {
  let (search, setSearch) = React.useState(_ => "");
  let (selected, setSelected) =
    React.useState(_ => Set.make(~id=(module Nft.KeyCompare)));

  let (hidden, setHidden) =
    // here we load, and at each set, we write.
    React.useState(_ =>
      switch (HiddenNftStorage.get()) {
      | Ok(l) => Set.fromArray(List.toArray(l), ~id=(module Nft.KeyCompare))
      | Error(_) => Set.make(~id=(module Nft.KeyCompare))
      }
    );
  let setHidden = f => {
    setHidden(set => {
      let set = f(set);
      HiddenNftStorage.set(set->Set.toList);
      set;
    });
  };

  let theme = ThemeContext.useTheme();

  let allTokensId =
    React.useMemo1(
      () => {
        nfts
        ->PublicKeyHash.Map.reduce(
            [],
            (acc, pkh, c) => {
              let ids =
                c.tokens
                ->Map.Int.reduce([], (acc, id, _) => [(pkh, id), ...acc]);
              List.concat(acc, ids);
            },
          )
        ->List.toArray
      },
      [|nfts|],
    );

  let checked =
    React.useMemo2(
      () => allTokensId->Array.size == Set.size(selected),
      (selected, allTokensId),
    );

  let filteredNfts =
    React.useMemo2(
      () =>
        nfts->TokenRegistry.Cache.keepTokens((_, _, token) =>
          token
          ->TokenRegistry.Cache.tokenName
          ->Option.mapWithDefault(false, name =>
              Js.String2.includes(name, search)
            )
        ),
      (search, allTokensId),
    );

  let (allSelectedHidden, noSelectedHidden) = {
    React.useMemo2(
      () => {
        selected->Set.reduce(
          (true, true),
          ((all, none), v) => {
            let currentHidden = hidden->Set.has(v);
            (all && currentHidden, none && !currentHidden);
          },
        )
      },
      (selected, hidden),
    );
  };

  let contracts =
    filteredNfts->PublicKeyHash.Map.map(contract =>
      <NftRowContract contract account selected setSelected hidden setHidden />
    )
    |> PublicKeyHash.Map.valuesToArray
    |> React.array;

  <>
    <ThemedTextInput
      style={Style.style(~flex=1., ())}
      icon=Icons.Search.build
      value=search
      onValueChange={value => setSearch(_ => value)}
      placeholder=I18n.input_placeholder#search_for_nft
    />
    <View
      style=Style.(
        array([|
          controlView,
          style(~borderColor=theme.colors.borderDisabled, ()),
        |])
      )>
      <CheckboxItem
        style=Style.(
          array([|
            NftRowToken.styles##checkboxMargin,
            style(~tintColor=theme.colors.iconPrimary, ()),
          |])
        )
        value=checked
        handleChange={checked => {
          checked
            ? setSelected(_ =>
                Set.fromArray(allTokensId, ~id=(module Nft.KeyCompare))
              )
            : setSelected(_ => Set.make(~id=(module Nft.KeyCompare)))
        }}
      />
      {ReactUtils.onlyWhen(
         <IconButton
           iconSizeRatio={5. /. 7.}
           style=Style.(
             array([|NftRowToken.styles##marginLeft10, styles##ctrlButton|])
           )
           icon=Icons.Eye.build
           onPress={_ => {
             let ids = Set.toArray(selected);
             setHidden(set => Set.removeMany(set, ids));
           }}
         />,
         allSelectedHidden
         && !selected->Set.isEmpty
         || !allSelectedHidden
         && !noSelectedHidden,
       )}
      {ReactUtils.onlyWhen(
         <IconButton
           style=Style.(
             array([|NftRowToken.styles##marginLeft10, styles##ctrlButton|])
           )
           iconSizeRatio={5. /. 7.}
           icon=Icons.EyeStrike.build
           onPress={_ =>
             setHidden(set => {
               let ids = Set.toArray(selected);
               Set.mergeMany(set, ids);
             })
           }
         />,
         noSelectedHidden
         && !selected->Set.isEmpty
         || !allSelectedHidden
         && !noSelectedHidden,
       )}
    </View>
    <ScrollView> contracts </ScrollView>
  </>;
};
