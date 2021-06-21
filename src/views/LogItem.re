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
      "itemContent":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingLeft=20.->dp,
          ~paddingVertical=10.->dp,
          ~width=100.->pct,
          (),
        ),
      "itemContentPage":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingLeft=0.->dp,
          ~width=100.->pct,
          (),
        ),
      "logBlock":
        style(~paddingVertical=12.->dp, ~marginHorizontal=8.->dp, ()),
      "actionButtons":
        style(~flexDirection=`row, ~flexShrink=0., ~marginLeft=auto, ()),
      "reqelt": style(~flexShrink=0., ~marginRight=16.->dp, ()),
      "itemError": style(~borderColor=Colors.error, ()),
      "itemInfo": style(~borderColor=Colors.valid, ()),
      "kindIcon": style(~marginRight=10.->dp, ()),
      "container": style(~borderRadius=3., ~marginTop=10.->dp, ()),
      "item":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~borderRadius=3.,
          ~borderLeftWidth=5.,
          ~flexWrap=`nowrap,
          ~width=100.->pct,
          ~alignSelf=`center,
          ~justifyContent=`flexStart,
          (),
        ),
      "button": style(~marginRight=4.->dp, ()),
      "toggleOff":
        style(~paddingLeft=32.->dp, ~width=100.->pct, ~paddingTop=0.->dp, ()),
      "reverseY": style(~transform=[|rotate(~rotate=180.->deg)|], ()),
    })
  );

module DeleteButton = {
  [@react.component]
  let make = (~isPrimary=?, ~style=?, ~indice, ~handleDelete) => {
    let onPress = _ => {
      handleDelete(indice);
    };
    <IconButton ?isPrimary ?style icon=Icons.Close.build onPress />;
  };
};

module OpenButton = {
  [@react.component]
  let make = (~isPrimary=?, ~opened, ~setOpened, ~chevronStyle) => {
    let onPress = _ => {
      setOpened(_ => !opened);
    };
    <IconButton
      ?isPrimary
      style=chevronStyle
      icon=Icons.ChevronDown.build
      onPress
    />;
  };
};

[@react.component]
let make =
    (
      ~indice,
      ~log: Logs.t,
      ~addToast,
      ~showTimestamp=true,
      ~handleDelete,
      ~isToast=false,
    ) => {
  let theme = ThemeContext.useTheme();

  let (opened, setOpened) = React.useState(_ => true);

  let kindStyle =
    switch (log.kind) {
    | Error => styles##itemError
    | Info => styles##itemInfo
    };

  let icon =
    switch (log.kind) {
    | Error => <Icons.CloseOutline size=16. color=Colors.error />
    | Info => <Icons.CheckOutline size=16. color=Colors.valid />
    };

  let chevronStyle = style_ =>
    opened ? style_ : Style.(array([|style_, styles##reverseY|]));

  let logDateContent =
    Js.Date.(log.timestamp->fromFloat->toUTCString)->React.string;

  let firstline =
    opened
      ? <Typography.InPageLog
          ellipsizeMode=`tail
          style={Style.style(~color=theme.colors.textMaxEmphasis, ())}
          fontWeightStyle=`light
          numberOfLines=1
          content={log.msg->React.string}
        />
      : React.null;

  let secondline =
    opened
      ? React.null
      : <Typography.InPageLog
          fontWeightStyle=`light
          style=Style.(
            array([|
              styles##itemContentPage,
              styles##toggleOff,
              style(~color=theme.colors.textMaxEmphasis, ()),
            |])
          )
          content={log.msg->React.string}
        />;

  let logsBackgroundColor =
    opened ? theme.colors.background : theme.colors.stateActive;

  isToast
    ? <View
        style=Style.(
          array([|
            styles##container,
            style(~backgroundColor=theme.colors.logBackground, ()),
          |])
        )>
        <Hoverable
          style=Style.(array([|styles##item, kindStyle|]))
          hoveredStyle={Style.style(
            ~backgroundColor=theme.colors.primaryStateHovered,
            (),
          )}>
          {_ => {
             <View style=styles##itemContent>
               <View style=styles##kindIcon> icon </View>
               {<Typography.Body2
                  style=Style.(
                    array([|
                      styles##reqelt,
                      style(
                        ~color=theme.colors.primaryTextMediumEmphasis,
                        (),
                      ),
                    |])
                  )
                  fontSize=12.
                  fontWeightStyle=`regular
                  numberOfLines=1>
                  "["->React.string
                  Js.Date.(log.timestamp->fromFloat->toLocaleString)
                  ->React.string
                  "]  -"->React.string
                </Typography.Body2>
                ->ReactUtils.onlyWhen(showTimestamp)}
               <Typography.Body1
                 style={Style.style(
                   ~color=theme.colors.primaryTextMediumEmphasis,
                   (),
                 )}
                 fontWeightStyle=`bold
                 ellipsizeMode=`tail
                 numberOfLines=1>
                 log.msg->React.string
               </Typography.Body1>
               <View style=styles##actionButtons>
                 {<ClipboardButton
                    isPrimary=true
                    data={log.msg}
                    copied=I18n.log#log_content
                    addToast
                    style=styles##button
                  />
                  ->ReactUtils.onlyWhen(log.kind == Error)}
                 <DeleteButton
                   isPrimary=true
                   indice
                   handleDelete
                   style=styles##button
                 />
               </View>
             </View>;
           }}
        </Hoverable>
      </View>
    : <View
        style=Style.(
          array([|
            styles##logBlock,
            style(
              ~borderBottomColor=theme.colors.stateDisabled,
              ~borderBottomWidth=1.,
              (),
            ),
            style(~backgroundColor=logsBackgroundColor, ()),
          |])
        )>
        <View>
          <View style=styles##itemContentPage>
            <OpenButton
              isPrimary=false
              opened
              setOpened
              chevronStyle={chevronStyle(styles##button)}
            />
            {<Typography.InPageLog
               style=Style.(
                 array([|
                   styles##reqelt,
                   style(~color=theme.colors.textMaxEmphasis, ()),
                 |])
               )
               numberOfLines=1
               fontWeightStyle=`bold
               content=logDateContent
             />
             ->ReactUtils.onlyWhen(showTimestamp)}
            firstline
            <View style=styles##actionButtons>
              {<ClipboardButton
                 isPrimary=false
                 data={log.msg}
                 copied=I18n.log#log_content
                 addToast
                 style=styles##button
               />
               ->ReactUtils.onlyWhen(log.kind == Error)}
              <DeleteButton
                isPrimary=false
                indice
                handleDelete
                style=styles##button
              />
            </View>
          </View>
          secondline
        </View>
      </View>;
};
