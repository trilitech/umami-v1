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

module TableHeader = {
  [@react.component]
  let make = () => {
    <Table.Head>
      <TokenRowItem.CellStandard>
        <Typography.Overline3>
          I18n.token_column_standard->React.string
        </Typography.Overline3>
      </TokenRowItem.CellStandard>
      <TokenRowItem.CellName>
        <Typography.Overline3>
          I18n.token_column_name->React.string
        </Typography.Overline3>
      </TokenRowItem.CellName>
      <TokenRowItem.CellSymbol>
        <Typography.Overline3>
          I18n.token_column_symbol->React.string
        </Typography.Overline3>
      </TokenRowItem.CellSymbol>
      <TokenRowItem.CellAddress>
        <Typography.Overline3>
          I18n.token_column_address->React.string
        </Typography.Overline3>
      </TokenRowItem.CellAddress>
      <TokenRowItem.CellTokenId>
        <Typography.Overline3>
          I18n.token_column_tokenid->React.string
        </Typography.Overline3>
      </TokenRowItem.CellTokenId>
      <TokenRowItem.CellAction> React.null </TokenRowItem.CellAction>
    </Table.Head>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "list": style(~paddingTop=4.->dp, ()),
      "headline": style(~fontSize=16., ()),
    })
  );

let makeRowItem = ((token, registered)) =>
  <TokenRowItem key={token->TokensLibrary.Token.uniqueKey} token registered />;

[@react.component]
let make = (~tokens) =>
  <>
    <TableHeader />
    <View style=styles##list>
      {tokens
       ->TokensLibrary.Generic.valuesToArray
       ->Array.map(makeRowItem)
       ->React.array}
    </View>
  </>;
