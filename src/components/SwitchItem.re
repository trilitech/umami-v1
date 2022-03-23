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
      "margin": style(~marginLeft=10.->dp, ()),
      "pressable":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingVertical=8.->dp,
          ~marginVertical=10.->dp,
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
      ~disabled=false,
      ~style as styleFromProp: option(Style.t)=?,
    ) => {
  <Pressable_
    style={_ =>
      Style.arrayOption([|Some(styles##pressable), styleFromProp|])
    }
    onPress={_ => setValue(_ => !value)}
    disabled
    accessibilityRole=`checkbox>
    {({hovered, pressed, focused}) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <>
         <ThemedSwitch value hovered pressed focused disabled />
         <Typography.Body1 style=styles##margin>
           label->React.string
         </Typography.Body1>
       </>;
     }}
  </Pressable_>;
};
