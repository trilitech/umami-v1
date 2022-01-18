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
    let style = Style.(style(~flexBasis=110.->dp, ~marginLeft=10.->dp, ()));
  });

module CellName =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=200.->dp, ()));
  });

module CellSymbol =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=100.->dp, ()));
  });

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(
        style(
          ~flexBasis=380.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flexShrink=1.,
          (),
        )
      );
  });

module CellTokenId =
  Table.MakeCell({
    let style =
      Style.(
        style(~flexBasis=100.->dp, ~flexGrow=1., ~alignItems=`flexStart, ())
      );
  });

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
  });

let styles =
  Style.(
    StyleSheet.create({
      "kind":
        style(
          ~paddingHorizontal=12.->dp,
          ~paddingVertical=2.->dp,
          ~fontSize=10.,
          (),
        ),
      "actions":
        style(~display=`flex, ~flexDirection=`row, ~alignItems=`center, ()),
      "iconOffset": style(~paddingLeft=14.->dp, ()),
      "copyButton": style(~marginRight=6.->dp, ()),
    })
  );

module EditionModal = {
  [@react.component]
  let make = (~action, ~item, ~token, ~tokens, ~currentChain) => {
    open TokensLibrary.Token;
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();
    <>
      {item(onPress)}
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView
          action
          address={token->address}
          kind={TokenContract.toTokenKind(token->kind, token->id)}
          chain={token->chain->Option.default(currentChain)}
          tokens
          closeAction
        />
      </ModalAction>
    </>;
  };
};

module AddButton = {
  [@react.component]
  let make = (~token, ~tokens, ~currentChain) => {
    let disabled = currentChain == None;

    let item = onPress => {
      let icon = (~color=?, ~style=?) =>
        Icons.SwitchOff.build(
          ~style=Style.arrayOption([|style, styles##iconOffset->Some|]),
          ~color?,
        );

      let tooltip = (
        "add_token_button" ++ token->TokensLibrary.Token.uniqueKey,
        I18n.Tooltip.add_to_wallet,
      );

      <IconButton disabled tooltip onPress icon size=46. iconSizeRatio=1.1 />;
    };

    <EditionModal
      action=`Add
      item
      token
      tokens
      currentChain={currentChain->Option.default("")}
    />;
  };
};

module MoreMenu = {
  module EditItem = {
    [@react.component]
    let make = (~token, ~tokens, ~currentChain) => {
      let disabled = currentChain == None;

      let item = onPress =>
        <Menu.Item
          disabled
          text=I18n.Menu.see_metadata
          onPress
          icon=Icons.Search.build
        />;

      <>
        <EditionModal
          action=`Edit
          item
          token
          tokens
          currentChain={currentChain->Option.default("")}
        />
      </>;
    };
  };

  module ContractLinkItem = {
    [@react.component]
    let make = (~token: TokensLibrary.Token.t, ~currentChain) => {
      let addToast = LogsContext.useToast();
      let disabled = currentChain == None;

      let onPress = _ => {
        let explorer =
          // EndpointError will actually never been used, since the menu item is
          // disabled in that case
          currentChain->Option.mapDefault(Error(Network.EndpointError), chain =>
            chain->Network.fromChainId->Network.externalExplorer
          );
        switch (explorer) {
        | Ok(url) =>
          System.openExternal(
            url ++ (token->TokensLibrary.Token.address :> string),
          )
        | Error(err) => addToast(Logs.error(~origin=Operation, err))
        };
      };

      <Menu.Item
        disabled
        icon=Icons.OpenExternal.build
        text=I18n.Menu.view_in_explorer
        onPress
      />;
    };
  };

  [@react.component]
  let make = (~token, ~tokens, ~currentChain) =>
    <Menu
      icon=Icons.More.build
      size=46.
      iconSizeRatio=0.5
      keyPopover={"tokenMenu" ++ token->TokensLibrary.Token.uniqueKey}>
      [|
        <EditItem token tokens currentChain />,
        <ContractLinkItem token currentChain />,
      |]
    </Menu>;
};

module RemoveButton = {
  [@react.component]
  let make = (~token: TokensLibrary.Token.t, ~currentChain) => {
    let theme = ThemeContext.useTheme();
    let (_tokenRequest, deleteToken) = StoreContext.Tokens.useDelete(false);
    let disabled = currentChain == None;

    let onPress = _e => {
      switch (token) {
      | Full(token) => deleteToken(token)->Promise.ignore
      | Partial(_, _, _) => ()
      };
    };

    let icon = (~color as _=?, ~style=?) =>
      Icons.SwitchOn.build(
        ~style=Style.arrayOption([|style, styles##iconOffset->Some|]),
        ~color=theme.colors.iconPrimary,
      );

    let tooltip = (
      "remove_token" ++ token->TokensLibrary.Token.uniqueKey,
      I18n.Btn.delete_token,
    );

    <IconButton disabled tooltip icon onPress iconSizeRatio=1.1 size=46. />;
  };
};

module Actions = {
  [@react.component]
  let make = (~token, ~registered: bool, ~currentChain, ~tokens) =>
    registered
      ? <View style=styles##actions>
          <MoreMenu token tokens currentChain />
          <RemoveButton token currentChain />
        </View>
      : <AddButton token tokens currentChain />;
};

[@react.component]
let make =
    (
      ~style=?,
      ~token: TokensLibrary.Token.t,
      ~registered: bool,
      ~currentChain,
      ~tokens,
    ) => {
  open TokensLibrary.Token;
  let theme = ThemeContext.useTheme();
  let addToast = LogsContext.useToast();

  let tokenId =
    switch (token->kind) {
    | `KFA1_2 => I18n.na
    | `KFA2 => token->id->Int.toString
    };
  <View
    style={Style.arrayOption([|
      style,
      Style.style(
        ~borderColor=theme.colors.stateDisabled,
        ~borderBottomWidth=1.,
        (),
      )
      ->Some,
    |])}>
    <Table.Row.Base height=44.>
      <CellStandard>
        <Tag
          contentStyle=styles##kind
          borderRadius=11.
          content={token->kind->TokenContract.kindToString}
        />
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
        <ClipboardButton
          copied=I18n.Log.address
          addToast
          tooltipKey={token->uniqueKey}
          data=(token->address :> string)
          style=styles##copyButton
        />
      </CellAddress>
      <CellTokenId>
        <Typography.Body1 numberOfLines=1>
          tokenId->React.string
        </Typography.Body1>
      </CellTokenId>
      <CellAction>
        <Actions registered token tokens currentChain />
      </CellAction>
    </Table.Row.Base>
  </View>;
};
