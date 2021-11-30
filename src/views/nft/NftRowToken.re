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
      "flexRowSpaceBetween": style(~justifyContent=`spaceBetween, ()),
      "itemsGroup":
        style(~display=`flex, ~flexDirection=`row, ~alignItems=`center, ()),
      "flex_row":
        style(
          ~display=`flex,
          ~height=44.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~borderTopWidth=1.,
          (),
        ),
      "checkboxMargin":
        style(~marginRight=(-5.)->dp, ~marginLeft=18.->dp, ()),
      "marginLeft10": style(~marginLeft=10.->dp, ()),
      "marginAuto": style(~margin=auto, ()),
      "image":
        style(~marginLeft=10.->dp, ~width="1.8vw", ~height="1.8vw", ()),
    })
  );

[@react.component]
let make =
    (
      ~nft: Token.t,
      ~account,
      ~address,
      ~selected,
      ~setSelected,
      ~hidden,
      ~setHidden,
    ) => {
  let source = NftElements.useNftSource(nft, NftFilesManager.getThumbnailURL);

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let theme = ThemeContext.useTheme();

  let tooltip = (nft.alias, I18n.btn#view_nft);
  let id = TokenRepr.id(nft);
  let hidden =
    React.useMemo1(
      () => TokenRegistry.Registered.isHidden(hidden, address, id),
      [|hidden|],
    );

  let isSelected =
    React.useMemo1(
      () => {NftSelection.isSelected(selected, address, id)},
      [|selected|],
    );

  <View
    style=Style.(
      array([|
        styles##flex_row,
        styles##flexRowSpaceBetween,
        style(~borderColor=theme.colors.stateDisabled, ()),
      |])
    )>
    <View style=styles##itemsGroup>
      <CheckboxItem
        style=styles##checkboxMargin
        value=isSelected
        handleChange={checked => {
          checked
            ? setSelected(address, id, true)
            : setSelected(address, id, false)
        }}
      />
      <IconButton
        icon={hidden ? Icons.EyeStrike.build : Icons.Eye.build}
        iconSizeRatio={5. /. 7.}
        onPress={_ =>
          hidden
            ? setHidden(NftSelection.singleton(address, id), false)
            : setHidden(NftSelection.singleton(address, id), true)
        }
        style=styles##marginLeft10
      />
      {source->Option.mapDefault(<SVGIconNoImg />, source =>
         <Image style=styles##image source resizeMode=`stretch />
       )}
      <Typography.Body1 style=styles##marginLeft10>
        nft.alias->React.string
      </Typography.Body1>
    </View>
    <View style=styles##itemsGroup>
      <Typography.Body1>
        {I18n.label#token_id(id |> Int.toString)->React.string}
      </Typography.Body1>
      <IconButton
        onPress={_ => openAction()}
        icon=Icons.MagnifierPlus.build
        iconSizeRatio={5. /. 7.}
        tooltip
        style=styles##marginLeft10
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <NftImageView closeAction nft account />
      </ModalAction>
    </View>
  </View>;
};
