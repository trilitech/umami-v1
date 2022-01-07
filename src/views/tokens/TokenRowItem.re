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

module CellStandard =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=60.->dp, ()));
    ();
  });

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
  });

module CellTokenId =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=100.->dp, ~alignItems=`flexStart, ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
    ();
  });

module TokenDeleteButton = {
  [@react.component]
  let make = (~token: TokensLibrary.Token.t) => {
    let (tokenRequest, deleteToken) = StoreContext.Tokens.useDelete();

    let onPressConfirmDelete = _e => {
      switch (token) {
      | Full(token) => deleteToken(token)->Promise.ignore
      | Partial(_, _, _) => ()
      };
    };

    <DeleteButton.IconButton
      tooltip=(
        "delete_token" ++ token->TokensLibrary.Token.uniqueKey,
        I18n.Btn.delete_token,
      )
      modalTitle=I18n.Title.delete_token
      onPressConfirmDelete
      request=tokenRequest
    />;
  };
};

[@react.component]
let make = (~token: TokensLibrary.Token.t, ~registered: bool) => {
  open TokensLibrary.Token;
  let _ = registered;
  let tokenId =
    switch (token->kind) {
    | `KFA1_2 => I18n.na
    | `KFA2 => token->id->Int.toString
    };
  <Table.Row>
    <CellStandard>
      <Tag content={token->kind->TokenContract.kindToString} />
    </CellStandard>
    <CellName>
      <Typography.Body1 numberOfLines=1>
        {token->name->Option.default("")->React.string}
      </Typography.Body1>
    </CellName>
    <CellSymbol>
      <Typography.Body1 numberOfLines=1>
        {token->symbol->Option.default("")->React.string}
      </Typography.Body1>
    </CellSymbol>
    <CellAddress>
      <Typography.Address numberOfLines=1>
        (token->address :> string)->React.string
      </Typography.Address>
    </CellAddress>
    <CellTokenId>
      <Typography.Body1 numberOfLines=1>
        tokenId->React.string
      </Typography.Body1>
    </CellTokenId>
    <CellAction> <TokenDeleteButton token /> </CellAction>
  </Table.Row>;
};
