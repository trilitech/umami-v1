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
      "container":
        StyleSheet.flatten([|
          FormStyles.square(40.),
          style(
            ~alignItems=`center,
            ~justifyContent=`center,
            ~borderRadius=20.,
            (),
          ),
          ShadowStyles.button,
        |]),
      "syncStatus":
        style(
          ~flexDirection=`row,
          ~minWidth=74.->dp,
          ~alignItems=`center,
          (),
        ),
    })
  );

type state =
  | NotInitiated
  | Loading(float)
  | Done;

module SyncButton = {
  [@react.component]
  let make = (~state, ~onRefresh, ~onStop) => {
    let theme = ThemeContext.useTheme();

    let (tooltip, icon, onPress) =
      switch (state) {
      | Loading(_) => (
          I18n.tooltip#stop_sync,
          Icons.Stop.build,
          (_ => onStop()),
        )
      | Done
      | NotInitiated => (
          I18n.tooltip#sync,
          Icons.SyncNFT.build,
          (_ => onRefresh()),
        )
      };

    <View
      style=Style.(
        array([|
          styles##container,
          style(~backgroundColor=theme.colors.elevatedBackground, ()),
        |])
      )>
      <Tooltip keyPopover="sync_button" text=tooltip>
        {(~pressableRef) => <IconButton pressableRef size=40. icon onPress />}
      </Tooltip>
    </View>;
  };
};

module SyncStatus = {
  [@react.component]
  let make = (~state) => {
    let theme = ThemeContext.useTheme();

    <View style=styles##syncStatus>
      {switch (state) {
       | Loading(percentage) =>
         <>
           <ActivityIndicator
             animating=true
             size=ActivityIndicator_Size.small
             color={theme.colors.textPositive}
             style=Style.(style(~padding=8.->dp, ()))
           />
           <Typography.Body1 colorStyle=`positive>
             {Format.sprintf("%.0f%%", percentage)->React.string}
           </Typography.Body1>
         </>
       | Done =>
         <>
           <Icons.Ok.I
             size=20.
             color={theme.colors.textPositive}
             style=Style.(style(~padding=8.->dp, ~paddingRight=8.->dp, ()))
           />
           <Typography.Body1 colorStyle=`positive>
             "100%"->React.string
           </Typography.Body1>
         </>
       | _ => React.null
       }}
    </View>;
  };
};

[@react.component]
let make = (~state, ~onRefresh, ~onStop) => {
  <View style={Style.style(~flexDirection=`row, ())}>
    <SyncButton state onRefresh onStop />
    <SyncStatus state />
  </View>;
};
