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
      "text": style(),
      "paragraph": style(~marginBottom=8.->dp, ()),
      "bullet":
        style(~fontSize=24., ()),
      "point":
        style(
          ~flex=1.,
          ~paddingLeft=12.->dp,
          ~marginTop=6.->dp,
          (),
        ),
    })
  );

module Body = {
  [@react.component]
  let make = (~text) => {
    <Typography.Body1 colorStyle=`highEmphasis style=styles##paragraph>
      text->React.string
    </Typography.Body1>;
  };
};

module BulletPoint = {
  [@react.component]
  let make = (~text) => {
    <View
      style=Style.(
        style(
          ~flexDirection=`row,
          ~marginLeft=8.->dp,
          ~marginRight=8.->dp,
          (),
        )
      )>
      <Typography.Subtitle2 colorStyle=`highEmphasis style=styles##bullet>
        {js|\u2022|js}->React.string
      </Typography.Subtitle2>
      <Typography.Subtitle2 colorStyle=`highEmphasis style=styles##point>
        text->React.string
      </Typography.Subtitle2>
    </View>;
  };
};

/* This should be generated from docs/standard/eula.md in the future. */
[@react.component]
let make = () =>
  <>
    <Body
      text={|You are now being redirected to wert.io ("Wert").
Please read this disclaimer carefully before using the Wert website operated by SHA2 Solutions OU (Reg. code 14952299).
Please note that SHA2 Solutions OU which operates Wert is a company separated from the Umami wallet, Nomadic Labs and its affiliates.
By continuing on Wert webpage, you acknowledge that you understand and expressely agree to the following terms and conditions:|}
    />
    <BulletPoint text={js|Wert is a virtual currency provider.|js} />
    <BulletPoint
      text={js|Umami wallet, Nomadic Labs and its affiliates do not make any guarantees for the Wert material, product or service offerings.|js}
    />
    <BulletPoint
      text={js|Umami wallet, Nomadic Labs and its affiliates cannot be held responsible for any information provided on the Wert website and their affiliates. The material and information contained on this website is for general information purposes only. You should not rely upon the material or information on the website as a basis for making any business, legal or any other decision.|js}
    />
    <BulletPoint
      text={js|Umami wallet, Nomadic Labs and its affiliates cannot be held responsible for the content and hyperlinks contained on the Wert website, for any damages or loss caused by the performance of the Wert website and their affiliates.|js}
    />
    <BulletPoint
      text={js|Wert may collect your personnal data in order for you to complete the purchase of virtual currency in compliance with the relevant laws and regulations.|js}
    />
    <BulletPoint
      text={js|Any personal data that might be collected by Wert is not collected or recorded by the Umami wallet, Nomadic Labs or its affiliates.|js}
    />
    <BulletPoint
      text={js|Umami wallet, Nomadic Labs and its affiliates cannot be held responsible for any breach of personal data, loss or damage on the Wert website and their affiliates.|js}
    />
    <BulletPoint
      text={js|By continuing with Wert services, you expressely consent to the fact that the Umami wallet will submit your public key for your account to Wert, in order for you to receive your purchased cryptocurrency.|js}
    />
    <BulletPoint
      text={js|Umami wallet, Nomadic Labs and its affiliates cannot be held responsible for any loss, damage or loss of profit related to the purchase of the virtual currency.|js}
    />
  </>;
