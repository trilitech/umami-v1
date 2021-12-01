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
      "pressable":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingVertical=6.->dp,
          ~paddingLeft=6.->dp,
          ~paddingRight=9.->dp,
          ~borderRadius=5.,
          (),
        ),
      "icon": style(~marginRight=4.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~text,
      ~onPress,
      ~tooltip=?,
      ~disabled=?,
      ~style=?,
      ~icon: Icons.builder,
      ~primary=false,
    ) => {
  let theme = ThemeContext.useTheme();

  let style = Style.arrayOption([|styles##pressable->Some, style|]);

  let pressableElement = (~pressableRef) =>
    <ThemedPressable
      ?pressableRef style ?disabled onPress accessibilityRole=`button>
      {icon(
         ~style=styles##icon,
         ~size=15.5,
         ~color=
           primary
             ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis,
       )}
      <Typography.ButtonSecondary
        style=Style.(
          style(
            ~color=?{
              primary ? Some(theme.colors.iconPrimary) : None;
            },
            (),
          )
        )>
        text->React.string
      </Typography.ButtonSecondary>
    </ThemedPressable>;

  switch (tooltip) {
  | Some((keyPopover, text)) =>
    <Tooltip keyPopover text>
      {(
         (~pressableRef) =>
           pressableElement(~pressableRef=Some(pressableRef))
       )}
    </Tooltip>
  | None => pressableElement(~pressableRef=None)
  };
};
