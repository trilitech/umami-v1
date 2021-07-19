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

module Radio = {
  [@react.component]
  let make = (~value: bool=false) => {
    let theme = ThemeContext.useTheme();
    let size = 16.;
    let color = {
      value ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;
    };
    value
      ? {
        <Icons.RadioOn size color />;
      }
      : {
        <Icons.RadioOff size color />;
      };
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "pressable":
        style(
          ~alignSelf=`flexStart,
          ~height=37.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          (),
        ),
      "radioContainer":
        style(
          ~marginRight=11.->dp,
          ~marginLeft=(-5.)->dp,
          ~height=30.->dp,
          ~width=30.->dp,
          ~borderRadius=15.,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~setValue: ('a => 'a) => unit,
      ~currentValue,
      ~disabled=false,
      ~style as styleFromProp: option(Style.t)=?,
    ) => {
  <Pressable_
    style={_ =>
      Style.arrayOption([|Some(styles##pressable), styleFromProp|])
    }
    onPress={_ => setValue(_ => value)}
    disabled
    accessibilityRole=`checkbox>
    {({hovered, pressed, focused}) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <>
         <ThemedPressable.ContainerInteractionState.Outline
           hovered pressed focused disabled style=styles##radioContainer>
           <Radio value={currentValue == value} />
         </ThemedPressable.ContainerInteractionState.Outline>
         <Typography.Body1> label->React.string </Typography.Body1>
       </>;
     }}
  </Pressable_>;
};
