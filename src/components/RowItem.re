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

module NestedElement = {
  let styles =
    Style.(
      StyleSheet.create({
        "nestedContainer": style(~width=15.->dp, ~marginRight=1.->dp, ()),
        "nestedBarVertical":
          style(
            ~width=2.->dp,
            ~position=`absolute,
            ~left=0.->dp,
            ~top=0.->dp,
            ~bottom=0.->dp,
            (),
          ),
        "nestedBarVerticalLast": style(~bottom=50.->pct, ()),
        "nestedBarHorizontal":
          style(
            ~height=2.->dp,
            ~position=`absolute,
            ~left=2.->dp,
            ~right=0.->dp,
            ~top=50.->pct,
            (),
          ),
        "nestedBarHorizontalLast": style(~left=0.->dp, ()),
      })
    );

  [@react.component]
  let make = (~isLast=false) => {
    let theme = ThemeContext.useTheme();

    <View style=styles##nestedContainer>
      <View
        style=Style.(
          arrayOption([|
            Some(styles##nestedBarVertical),
            isLast ? Some(styles##nestedBarVerticalLast) : None,
            Some(style(~backgroundColor=theme.colors.borderDisabled, ())),
          |])
        )
      />
      <View
        style=Style.(
          arrayOption([|
            Some(styles##nestedBarHorizontal),
            isLast ? Some(styles##nestedBarHorizontalLast) : None,
            Some(style(~backgroundColor=theme.colors.borderDisabled, ())),
          |])
        )
      />
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flexDirection=`row, ()),
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "innerContainer":
        style(~flexDirection=`row, ~flex=1., ~borderRadius=5., ()),
      "border":
        style(
          ~marginVertical=6.->dp,
          ~width=2.->dp,
          ~borderTopRightRadius=2.,
          ~borderBottomRightRadius=2.,
          (),
        ),
    })
  );

module Base = {
  [@react.component]
  let make = (~height, ~style as stylearg=?, ~children) => {
    let theme = ThemeContext.useTheme();
    <Hoverable
      style=Style.(
        arrayOption([|
          stylearg,
          Some(styles##container),
          Some(style(~height=height->dp, ())),
        |])
      )>
      {hovered => {
         <View
           style=Style.(
             arrayOption([|
               Some(styles##innerContainer),
               hovered
                 ? Some(
                     style(~backgroundColor=theme.colors.stateHovered, ()),
                   )
                 : None,
             |])
           )>
           {children(hovered)}
         </View>;
       }}
    </Hoverable>;
  };
};

module Bordered = {
  [@react.component]
  let make = (~height, ~style=?, ~isNested=false, ~isLast=false, ~children) => {
    let theme = ThemeContext.useTheme();
    <Base height ?style>
      {_ =>
         <>
           {isNested ? <NestedElement isLast /> : React.null}
           <View
             style=Style.(
               array([|
                 styles##border,
                 style(~backgroundColor=theme.colors.borderHighEmphasis, ()),
               |])
             )
           />
           <View style=styles##inner> children </View>
         </>}
    </Base>;
  };
};
