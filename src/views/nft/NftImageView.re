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
      "content":
        style(
          ~display=`flex,
          ~flexDirection=`column,
          ~maxWidth=550.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
      "container":
        style(
          ~flex=1.,
          ~display=`flex,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
      "image":
        style(
          ~height=550.->dp,
          ~width=550.->dp,
          ~paddingTop=20.->dp,
          ~paddingBottom=20.->dp,
          ~alignItems=`center,
          (),
        ),
      "actions":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~maxWidth=1000.->dp,
          ~width=120.->pct,
          ~marginTop=100.->dp,
          ~alignItems=`flexStart,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "infos": style(~display=`flex, ~flexDirection=`column, ()),
      "buttons": style(~flexDirection=`row, ~alignItems=`center, ()),
      "descr": style(~marginTop=24.->dp, ~maxWidth=380.->dp, ()),
      "specs": style(~marginRight=32.->dp, ()),
    })
  );

[@react.component]
let make = (~closeAction, ~account, ~nft: Token.t) => {
  let source = NftElements.useNftSource(nft, NftFilesManager.getDisplayURL);

  let (visibleSendModal, openSendModal, closeSendModal) =
    ModalAction.useModalActionState();

  let (visibleSpecsModal, openSpecsModal, closeSpecsModal) =
    ModalAction.useModalActionState();

  let (_, animatedOpenValue) =
    AnimationHooks.useAnimationOpen(true, () => ());

  <ModalFormView.Large closing={ModalFormView.Close(closeAction)}>
    <View style=styles##container>
      <View style=styles##content>
        <View
          style=Style.(
            array([|
              style(~opacity=animatedOpenValue->Animated.StyleProp.float, ()),
              styles##image,
            |])
          )>
          {source->Option.mapDefault(<SVGIconNoImg />, source =>
             <Image
               style=Style.(style(~width=100.->pct, ~height=100.->pct, ()))
               source
               resizeMode=`contain
             />
           )}
        </View>
        <View style=styles##actions>
          <View style=styles##infos>
            <Typography.Headline>
              nft.alias->React.string
            </Typography.Headline>
            {nft.asset.description
             ->ReactUtils.mapOpt(d =>
                 <Typography.Body1 style=styles##descr>
                   d->React.string
                 </Typography.Body1>
               )}
          </View>
          <View style=styles##buttons>
            <Buttons.SubmitTertiary
              style=styles##specs
              onPress={_ => openSpecsModal()}
              text=I18n.Btn.view_specs
            />
            <Buttons.SubmitPrimary
              onPress={_ => openSendModal()}
              text=I18n.Btn.send
            />
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
  </ModalFormView.Large>;
};
