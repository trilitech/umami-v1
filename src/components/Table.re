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

module Head = {
  let styles =
    Style.(
      StyleSheet.create({
        "thead":
          style(
            ~flexDirection=`row,
            ~alignItems=`center,
            ~height=30.->dp,
            ~paddingLeft=22.->dp,
            ~borderBottomWidth=1.,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children) => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        array([|
          styles##thead,
          style(~borderColor=theme.colors.borderDisabled, ()),
        |])
      )>
      children
    </View>;
  };
};

module Row = {
  let styles =
    Style.(StyleSheet.create({"borderSpacer": style(~width=20.->dp, ())}));

  [@react.component]
  let make = (~style=?, ~children) => {
    <RowItem.Bordered height=48.>
      <View
        style={Style.arrayOption([|Some(styles##borderSpacer), style|])}
      />
      children
    </RowItem.Bordered>;
  };
};

module Empty = {
  let styles =
    Style.(
      StyleSheet.create({
        "empty": style(~paddingLeft=22.->dp, ~paddingTop=14.->dp, ()),
      })
    );

  [@react.component]
  let make = (~children) => {
    <Typography.Body1 style=styles##empty> children </Typography.Body1>;
  };
};

module Cell = {
  let styles =
    Style.(
      StyleSheet.create({
        "cell":
          style(~flexShrink=0., ~minWidth=75.->dp, ~marginRight=24.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~style as styleFromProp: ReactNative.Style.t,
        ~children: option(React.element)=?,
      ) => {
    <View style={Style.array([|styles##cell, styleFromProp|])} ?children />;
  };
};

module type StyleForCell = {let style: Style.t;};

module MakeCell = (CustomStyle: StyleForCell) => {
  let makeProps = Cell.makeProps(~style=CustomStyle.style);
  let make = Cell.make;
};
