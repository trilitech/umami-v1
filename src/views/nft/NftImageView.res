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
    "content": style(
      ~display=#flex,
      ~flexDirection=#column,
      ~maxWidth=550.->dp,
      ~justifyContent=#center,
      ~alignItems=#center,
      (),
    ),
    "container": style(~flex=1., ~display=#flex, ~justifyContent=#center, ~alignItems=#center, ()),
    "image": style(
      ~height=550.->dp,
      ~width=550.->dp,
      ~paddingTop=20.->dp,
      ~paddingBottom=20.->dp,
      ~alignItems=#center,
      (),
    ),
    "actions": style(
      ~display=#flex,
      ~flexDirection=#row,
      ~maxWidth=1000.->dp,
      ~width=120.->pct,
      ~marginTop=100.->dp,
      ~alignItems=#flexStart,
      ~justifyContent=#spaceBetween,
      (),
    ),
    "infos": style(~display=#flex, ~flexDirection=#column, ()),
    "buttons": style(~flexDirection=#row, ~alignItems=#center, ()),
    "descr": style(~marginTop=24.->dp, ~maxWidth=380.->dp, ()),
    "specs": style(~marginRight=32.->dp, ()),
  })
}

module ReactiveImage = {
  @react.component
  let make = (~gallery=false, ~source: option<ReactNative.Image.Source.t>) => {
    let theme = ThemeContext.useTheme()
    let (img, setImg) = React.useState(() => { source->Option.mapDefault(#none, source => #loading(source))});
    let addLog = LogsContext.useAdd()
    switch img {
            | #none =>
            <SVGIconNoImg
              fill=theme.colors.iconDisabled
              height={ 100.->Style.pct }
            />
            | #error =>
            <>
            <SVGIconNft fill=theme.colors.textDisabled height={ 100.->Style.pct } />
            <View style={
                open Style
                gallery
                ? style(
                  ~position=#absolute,
                  ~right=15.->pct,
                  ~top=7.->pct,
                  ~width=30.->pct,
                  ())
                : style(
                  ~position=#absolute,
                  ~right=3.->pct,
                  ~top=2.->pct,
                  ~width=40.->pct,
                  ())
              }>
              <View style={
                open Style
                style(
                  ~position=#absolute,
                  ~left=2.->pct,
                  ~top=2.->pct,
                  ~right=2.->pct,
                  ~bottom=2.->pct,
                  ~backgroundColor="white",
                  ~borderRadius=100.,
                  ~zIndex=-1,
                 ())
              }></View>
              <SVGIconError height={ 25.->Style.pct }  />
            </View>
            </>
            | #loading(source) =>
            <>
            <Image
              onError={e => {
                  addLog(true, Logs.error(~origin=Logs.Nft, Errors.Generic(e.nativeEvent.error)));
                  setImg(_ => #error)}
              }
              onLoad={_ => setImg(_ => #loaded(source))}
              style={
                  open Style
                  style(~width=100.->pct, ~height=100.->pct, ())
                }
                source
                resizeMode=#contain
              />
              <ActivityIndicator
                animating=true
                size=ActivityIndicator_Size.small
                color=theme.colors.iconHighEmphasis
                style={
                  open Style
                  style(~position=#absolute, ~top=50.->pct, ~marginTop=(-10.)->pt, ())
                }
                />
              </>
            | #loaded(source) =>
            <Image
                style={
                  open Style
                  style(~width=100.->pct, ~height=100.->pct, ())
                }
                source
                resizeMode=#contain
              />
          }
  }
}

@react.component
let make = (~closeAction, ~account, ~nft: Token.t) => {
  let source = NftElements.useNftSource(nft, NftFilesManager.getDisplayURL)

  let (visibleSendModal, openSendModal, closeSendModal) = ModalAction.useModalActionState()

  let (visibleSpecsModal, openSpecsModal, closeSpecsModal) = ModalAction.useModalActionState()

  let (_, animatedOpenValue) = AnimationHooks.useAnimationOpen(true, () => ())

  <ModalFormView.Large closing=ModalFormView.Close(closeAction)>
    <View style={styles["container"]}>
      <View style={styles["content"]}>
        <View
          style={
            open Style
            array([
              style(~opacity=animatedOpenValue->Animated.StyleProp.float, ()),
              styles["image"],
            ])
          }>
          <ReactiveImage source/>
        </View>
        <View style={styles["actions"]}>
          <View style={styles["infos"]}>
            <Typography.Headline> {nft.alias->React.string} </Typography.Headline>
            {nft.asset.description->ReactUtils.mapOpt(d =>
              <Typography.Body1 style={styles["descr"]}> {d->React.string} </Typography.Body1>
            )}
          </View>
          <View style={styles["buttons"]}>
            <Buttons.SubmitTertiary
              style={styles["specs"]} onPress={_ => openSpecsModal()} text=I18n.Btn.view_specs
            />
            <Buttons.SubmitPrimary onPress={_ => openSendModal()} text=I18n.Btn.send />
          </View>
        </View>
      </View>
    </View>
    <ModalAction visible=visibleSpecsModal onRequestClose=closeSpecsModal>
      <NftSpecsView nft closeAction=closeSpecsModal />
    </ModalAction>
    <ModalAction visible=visibleSendModal onRequestClose=closeSendModal>
      <SendNFTView source=account nft closeAction=closeSendModal />
    </ModalAction>
  </ModalFormView.Large>
}
