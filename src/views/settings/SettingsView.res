/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

@react.component
let make = () =>
  <Page>
    {I18n.Title.settings->Typography.headline(~style=Styles.title)}
    <VerificationSettingView />
    <ThemeSettingView />
    <ChainSettingView />
    <BeaconSettingView />
    <BackupSettingView />
    {System.hasAutoUpdate() ? <UpdateSettingView /> : React.null}
    <DangerSettingView />
    {
      let tzkt =
        <ReactNative.Pressable onPress={_ => System.openExternal("https://tzkt.io/")}>
          {_ => "TzKT"->React.string}
        </ReactNative.Pressable>

      <SettingsComponents.Block isLast=true>
        <Typography.Body1> {I18n.Settings.about_ELEMENT(tzkt)} </Typography.Body1>
      </SettingsComponents.Block>
    }
  </Page>
