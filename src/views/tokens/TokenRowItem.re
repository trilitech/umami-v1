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

module CellName =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=132.->dp, ()));
    ();
  });

module CellSymbol =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=90.->dp, ()));
    ();
  });

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexGrow=1., ~flexShrink=1., ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
    ();
  });

module TokenDeleteButton = {
  [@react.component]
  let make = (~token: Token.t) => {
    let (tokenRequest, deleteToken) = StoreContext.Tokens.useDelete();

    let onPressConfirmDelete = _e => {
      deleteToken(token)->ignore;
    };

    <DeleteButton.IconButton
      tooltip=("delete_token", I18n.btn#delete_token)
      modalTitle=I18n.title#delete_token
      onPressConfirmDelete
      request=tokenRequest
    />;
  };
};

[@react.component]
let make = (~token: Token.t) => {
  <Table.Row>
    <CellName>
      <Typography.Body1 numberOfLines=1>
        token.alias->React.string
      </Typography.Body1>
    </CellName>
    <CellSymbol>
      <Typography.Body1 numberOfLines=1>
        token.symbol->React.string
      </Typography.Body1>
    </CellSymbol>
    <CellAddress>
      <Typography.Address numberOfLines=1>
        token.address->React.string
      </Typography.Address>
    </CellAddress>
    <CellAction> <TokenDeleteButton token /> </CellAction>
  </Table.Row>;
};
