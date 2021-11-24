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

let styles =
  Style.(
    StyleSheet.create({
      "innerCard":
        style(
          ~display=`flex,
          ~flexDirection=`column,
          ~alignItems=`center,
          (),
        ),
      "card":
        style(
          ~paddingTop="20px",
          ~flexBasis="content",
          ~flex=1.,
          ~maxWidth="18%",
          ~minWidth="18%",
          ~marginRight="1%",
          ~marginLeft="1%",
          (),
        ),
      "text":
        style(
          ~paddingTop="10px",
          ~paddingBottom="10px",
          ~paddingLeft="10px",
          ~justifyContent=`center,
          ~width="100%",
          (),
        ),
      "image":
        style(
          ~width=100.->pct,
          ~height=200.->dp,
          ~paddingTop=20.->dp,
          ~paddingBottom=20.->dp,
          ~alignItems=`center,
          (),
        ),
      "view":
        style(
          ~marginHorizontal=(-1.)->pct,
          ~flexWrap=`wrap,
          ~display=`flex,
          ~flexDirection=`row,
          (),
        ),
    })
  );

module Card = {
  [@react.component]
  let make = (~nft: Token.t, ~account: Account.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let (_, animatedOpenValue) =
      AnimationHooks.useAnimationOpen(true, () => ());

    let source =
      NftElements.useNftSource(nft, NftFilesManager.getThumbnailURL);
    let theme = ThemeContext.useTheme();
    <>
      <ThemedPressable.Base
        onPress={_ => openAction()}
        extStyle={_ => styles##card}
        style=styles##innerCard>
        <Animated.View
          style=Style.(
            array([|
              style(~opacity=animatedOpenValue->Animated.StyleProp.float, ()),
              styles##image,
              Style.(
                style(~backgroundColor=theme.colors.stateRowHovered, ())
              ),
            |])
          )>
          {source->Option.mapDefault(
             <SVGIconNoImg
               fill={theme.colors.iconDisabled}
               height=Style.(100.->pct)
             />,
             source =>
             <Image
               style=Style.(style(~width=100.->pct, ~height=100.->pct, ()))
               source
               resizeMode=`contain
             />
           )}
        </Animated.View>
        <View
          style=Style.(
            array([|
              styles##text,
              style(~backgroundColor=theme.colors.cardBackground, ()),
            |])
          )>
          <Typography.Notice> nft.alias->React.string </Typography.Notice>
        </View>
      </ThemedPressable.Base>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <NftImageView closeAction nft account />
      </ModalAction>
    </>;
  };
};

let uniqueKey = (contract: PublicKeyHash.t, id) =>
  (contract :> string) ++ "-" ++ Int.toString(id);

[@react.component]
let make = (~nfts: TokenRegistry.Cache.t) => {
  let account = StoreContext.SelectedAccount.useGet();
  let (search, setSearch) = React.useState(_ => "");
  let hidden = HiddenNftStorage.get()->Result.getWithDefault([]);
  let nfts =
    React.useMemo1(
      () =>
        nfts->TokenRegistry.Cache.keepTokens((pkh, id, _) =>
          !hidden->List.has((pkh, id), (a, b) => a == b)
        ),
      [|nfts|],
    );

  let nfts =
    React.useMemo1(
      () =>
        nfts->TokenRegistry.Cache.keepTokens((_, _, token) =>
          token
          ->TokenRegistry.Cache.tokenName
          ->Option.mapWithDefault(false, name =>
              Js.String2.includes(name, search)
            )
        ),
      [|nfts|],
    );

  let cards =
    React.useMemo1(
      () =>
        nfts
        ->TokenRegistry.Cache.valuesToArray
        ->Array.keepMap(
            fun
            | Partial(_, _) => None
            | Full(nft) =>
              account->Option.map(account =>
                <Card
                  key={uniqueKey(nft.address, TokenRepr.id(nft))}
                  nft
                  account
                />
              ),
          )
        ->React.array,
      [|nfts|],
    );

  <>
    <ThemedTextInput
      style=Style.(style(~flexBasis=48.->dp, ()))
      icon=Icons.Search.build
      value=search
      onValueChange={value => setSearch(_ => value)}
      placeholder=I18n.input_placeholder#search_for_nft
    />
    <DocumentContext.ScrollView>
      {nfts->PublicKeyHash.Map.isEmpty
         ? <NftEmptyView /> : <View style=styles##view> cards </View>}
    </DocumentContext.ScrollView>
  </>;
};
