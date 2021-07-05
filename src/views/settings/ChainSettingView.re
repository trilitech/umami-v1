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
open SettingsComponents;

module StateLenses = [%lenses
  type state = {network: [ | `Mainnet | `Florencenet | `Custom(string)]}
];
module ChainForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ()),
      "block": style(~flex=1., ~flexDirection=`column, ()),
      "leftcolumntitles": style(~justifyContent=`spaceBetween, ()),
      "chainSeparation": style(~marginTop=30.->dp, ()),
      "button": style(~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = SdkContext.useSettings();

  let writeNetwork = f => {
    let network = f(settings->AppSettings.network);
    let network =
      network == ConfigFile.Default.network ? None : Some(network);

    writeConf(c => {...c, network});
  };

  <Block title=I18n.settings#chain_title>
    <View accessibilityRole=`form style=styles##row>
      <ColumnLeft style=styles##leftcolumntitles>
        <RadioItem
          label=I18n.t#mainnet
          value=`Mainnet
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
        />
        <RadioItem
          label=I18n.t#florencenet
          value=`Florencenet
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
        />
      </ColumnLeft>
    </View>
  </Block>;
};
