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
          ~paddingTop=20.->dp,
          ~flexBasis="content",
          ~flex=1.,
          ~maxWidth=18.->pct,
          ~minWidth=18.->pct,
          ~marginRight=1.->pct,
          ~marginLeft=1.->pct,
          (),
        ),
      "text":
        style(
          ~paddingTop=16.->dp,
          ~paddingBottom=16.->dp,
          ~paddingLeft=12.->dp,
          ~justifyContent=`center,
          ~width=100.->pct,
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
  let make = (~nft: Token.t, ~balance: ReBigNumber.t, ~account: Account.t) => {
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
          <Typography.Headline style=Style.(style(~fontSize=16., ()))>
            nft.alias->React.string
          </Typography.Headline>
          <Typography.Body1 style=Style.(style(~marginTop=8.->dp, ()))>
            {I18n.label#editions(balance |> ReBigNumber.toString)
             ->React.string}
          </Typography.Body1>
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
let make = (~nfts: TokensLibrary.WithBalance.t) => {
  let account = StoreContext.SelectedAccount.useGet();
  let hidden =
    TokenStorage.Registered.get()
    ->Result.getWithDefault(PublicKeyHash.Map.empty);

  let nfts =
    React.useMemo1(
      () =>
        nfts->TokensLibrary.Generic.keepTokens((pkh, id, _) =>
          !hidden->RegisteredTokens.isHidden(pkh, id)
        ),
      [|nfts|],
    );

  let cards =
    React.useMemo1(
      () =>
        nfts
        ->TokensLibrary.Generic.valuesToArray
        ->Array.keepMap(
            fun
            | (Partial(_, _, _), _) => None
            | (Full(nft), balance) =>
              account->Option.map(account =>
                <Card
                  key={uniqueKey(nft.address, TokenRepr.id(nft))}
                  nft
                  balance
                  account
                />
              ),
          )
        ->React.array,
      [|nfts|],
    );

  <>
    <DocumentContext.ScrollView>
      {nfts->PublicKeyHash.Map.isEmpty
         ? <NftEmptyView /> : <View style=styles##view> cards </View>}
    </DocumentContext.ScrollView>
  </>;
};
