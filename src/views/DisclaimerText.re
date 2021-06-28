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
      "paragraphs": style(~marginTop=4.->dp, ~marginBottom=16.->dp, ()),
      "headings2": style(~paddingTop=4.->dp, ()),
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
  let make = (~lastParagraph=false, ~text) => {
    let style =
      Style.(
        array([|
          styles##paragraphs,
          lastParagraph ? style(~marginBottom=0.->dp, ()) : style(),
        |])
      );
    <Typography.Body1 colorStyle=`highEmphasis style>
      text->React.string
    </Typography.Body1>;
  };
};

/* This should be generated from docs/standard/eula.md in the future. */
[@react.component]
let make = () =>
  <>
    <Heading1 text="Umami Wallet End User License Agreement" />
    <Heading2 text="Agreement" />
    <Body
      text="By making use of Umami, User agrees to follow terms as set herein."
    />
    <Heading2 text="Umami Description" />
    <Body
      text={|The Umami Wallet (hereafter "Umami") is an open source software project supported by Nomadic Labs, a company incorporated in France. Umami is a simple account management system for Tezos' cryptocurrency (hereafter "tez").

Umami's intended purpose is to: (i) allow users to view their tez balances; (ii) facilitates the forming operations for submission to the blockchain; and (iii) allows users to save and organize their contacts' addresses.|}
    />
    <Heading1 text="Umami is not a VASP" />
    <Body
      text={|Umami is not a Virtual Asset Service Provider, as it does not provide the following services: (i) recommendations or advice; or (ii) trade or swap between tokens or assets.

Umami is free of charge. Umami does not profit from its operations, nor does Umami collect any user's personal data centrally.|}
    />
    <Heading1 text="No Guarantees" />
    <Body
      text={|As Umami is an open source project, the Umami Team members cannot guarantee that any errors of any nature that could possibly appear in the source code "have been seen." The Umami Team members, Nomadic Labs, its subsidiaries, the directors, employees and agents cannot under any circumstances be held liable for the use of and reliance by the Users or any third Party whatsoever on Umami.

Umami's source code reflects the best of our knowledge, it has been made in good faith and while every care has been taken in coding Umami. The Umami Team members, Nomadic Labs, its subsidiaries, the directors, employees and agents make no engagement and give no warranties of whatever nature with respect to the Umami source code, including but not limited to any bugs, faults or risk of loss of your crypto-assets.

No content on Umami shall constitute or shall be understood as a recommendation to enter any investment transactions or agreements of any kind, nor to use the code without having carried out your own technical audit.|}
    />
    <Heading1 text="No Advice and Appropriateness" />
    <Body
      text={|Before taking any action in using this "Software", User should consider Umami relevance to their particular circumstances, and User is strongly advised to consult a professional technical advisor, verified and reputable agent from a professional third party. The Umami Team members, Nomadic Labs, its subsidiaries, the directors, employees and agents do not make any warranties about the completeness, reliability and accuracy or fitness for lawful use of the software of the information presented herein.|}
    />
    <Heading1 text="Not a Subcontractor Contract" />
    <Body
      text={|Use of Umami and agreement of the terms set herein cannot be considered a subcontractor contract.|}
    />
    <Heading1 text="Hold Harmless" />
    <Body
      text={|User understands and agrees that any action on Umami is strictly at User's own risk. The Umami Team members, Nomadic Labs, its subsidiaries, the directors, employees and agents will not be held liable for any losses and damages in connection to the use of the Umami source code.|}
    />
    <Heading1 text="Lawful Use" />
    <Body
      lastParagraph=true
      text={|User shall comply with all applicable laws, statutes, regulations and codes relating to anti-bribery and anti-corruption, fight against the financing of terrorism and shall not engage in any activity, practice or conduct outside France, which would constitute an offence of the previous texts, if such activity, practice or conduct had been carried out inside France.
None of the Umami Team members, Nomadic Labs, its subsidiaries, the directors, employees and agents shall be considered liable of such misconduct by a malicious User.|}
    />
  </>;
