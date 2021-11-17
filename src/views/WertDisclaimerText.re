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
      "paragraphs":
        style(
          ~marginTop=0.->dp,
          ~marginBottom=16.->dp,
          ~textAlign=`center,
          (),
        ),
      "headings2": style(~paddingTop=4.->dp, ()),
      "bullet":
        style(
          ~fontSize=24.,
          ~marginTop=4.->dp,
          ~marginBottom=16.->dp,
          (),
        ),
        "point":
        style(
          ~flex=1.,
          ~paddingLeft=16.->dp,
          ~marginTop=10.->dp,
          ~marginBottom=16.->dp,
          (),
        ),
    })
  );

module Heading1 = {
  [@react.component]
  let make = (~text) =>
    <Typography.Overline2> text->React.string </Typography.Overline2>;
};

module Heading2 = {
  [@react.component]
  let make = (~text) =>
    <Typography.Overline3 style=styles##headings2>
      text->React.string
    </Typography.Overline3>;
};

module Body = {
  [@react.component]
  let make = (~lastParagraph=false, ~isBulletPoint=false, ~text) => {
    let style =
      Style.(
        array([|
          styles##paragraphs,
          lastParagraph ? style(~marginBottom=0.->dp, ()) : style(),
          isBulletPoint ? style(~textAlign=`left, ()) : style(),
        |])
      );
    <Typography.Subtitle2 colorStyle=`highEmphasis style>
      text->React.string
    </Typography.Subtitle2>;
  };
};

module BulletPoint = {
  [@react.component]
  let make = (~lastParagraph=false, ~text) => {
    let bullet =
      Style.(
        array([|
          styles##bullet,
          lastParagraph ? style(~marginBottom=0.->dp, ()) : style(),
        |])
      );
    let point =
      Style.(
        array([|
          styles##point,
          lastParagraph ? style(~marginBottom=0.->dp, ()) : style(),
        |])
      );
    <View style=Style.(style(~flexDirection=`row, ~alignItems=`flexStart, ~marginLeft=8.->dp, ~marginRight=8.->dp, ()))>
      <Typography.Subtitle2 colorStyle=`highEmphasis style=bullet>
        {js|\u2022|js}->React.string
      </Typography.Subtitle2>
      <Typography.Subtitle2 colorStyle=`highEmphasis style=point>
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
      text={|You are being directed to a Wert service ("Wert") that is separate from the Umami Wallet ("Umami"). Umami and its parent company, Nomadic Labs, do not make any guarantee for the Wert service.|}
    />
    <Body
      text={|Wert is a service for purchasing tez cryptocurrency ("tez"). Wert will request collection of personal data in order to complete a purchase whilst complying with relevant law and regulations.
Here is the relevant data policy for this service integration between Umami and Wert:|}
    />
    <BulletPoint
      text={js|any data submitted to Wert is not recorded by Umami|js}
    />
    <BulletPoint
      lastParagraph=true
      text={js|Umami submits your selected public key for your account, in order for you to receive your purchased tez|js}
    />
  </>;
