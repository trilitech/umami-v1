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
open ReCustomAuth;

module StateInfoView = {
  [@react.component]
  let make = (~closeAction, ~provider) => {
    let styles = InitLedgerView.styles;
    let providerName = provider->ReCustomAuth.getProviderName;

    let () = ReCustomAuthUtils.useDeeplinkHandler();

    <ModalFormView
      closing={ModalFormView.Close(closeAction)}
      title={I18n.Title.provider_authentification(providerName)}>
      {<View style=InitLedgerView.styles##content>
         <View
           style=Style.(
             array([|FormStyles.section##spacing, styles##loading|])
           )>
           <LoadingView size=ActivityIndicator_Size.large />
         </View>
         {I18n.Expl.provider_authentification(providerName)
          ->React.string
          ->InitLedgerView.expl}
       </View>}
    </ModalFormView>;
  };
};

module CustomAuthButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~margin=16.->dp,
            ~height=72.->dp,
            ~width=173.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~paddingHorizontal=16.->dp,
            ~borderRadius=4.,
            (),
          )
          ->unsafeAddStyle({
              "boxShadow": "0 1px 3px 0 rgba(0, 0, 0, 0.2), 0 2px 1px -1px rgba(0, 0, 0, 0.12), 0 1px 1px 0 rgba(0, 0, 0, 0.14)",
            }),
        "iconContainer":
          style(
            ~width=40.->dp,
            ~height=40.->dp,
            ~justifyContent=`center,
            ~alignItems=`center,
            ~marginRight=16.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~provider) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let (_request, setCustomAuth) =
      StoreContext.Accounts.useCustomAuthLogin(
        ~onClosedPopup=closeAction,
        (),
      );
    let onPress = _ => {
      openAction();
      setCustomAuth(provider)->Promise.get(_ => closeAction());
    };

    let theme = ThemeContext.useTheme();

    <>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <StateInfoView closeAction provider />
      </ModalAction>
      <ThemedPressable
        onPress
        style=Style.(
          array([|
            styles##container,
            style(~backgroundColor=theme.colors.barBackground, ()),
          |])
        )>
        <View style=Style.(array([|styles##iconContainer|]))>
          provider->CustomAuthProviders.getComponent
        </View>
        <Typography.Subtitle2>
          {provider->ReCustomAuth.getProviderName->React.string}
        </Typography.Subtitle2>
      </ThemedPressable>
    </>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~paddingVertical=38.->dp, ~alignItems=`center, ()),
      "title": style(~marginBottom=24.->dp, ()),
      "row": style(~flexDirection=`row, ~justifyContent=`center, ()),
    })
  );

[@react.component]
let make = () => {
  let (torusInitiated, setTorusInitiated) = React.useState(_ => false);

  React.useEffect1(
    () => {
      if (ReCustomAuth.flagOn) {
        torusSdk
        ->init(initParams(~skipSw=true, ()))
        ->Promise.tapOk(_ => setTorusInitiated(_ => true))
        ->Promise.ignore;
      };
      None;
    },
    [||],
  );

  torusInitiated
    ? <View style=styles##container>
        <Typography.Headline style=styles##title>
          I18n.Title.custom_auth_sign_in_up->React.string
        </Typography.Headline>
        <View style=styles##row> <CustomAuthButton provider=`google /> </View>
      </View>
    : React.null;
};
