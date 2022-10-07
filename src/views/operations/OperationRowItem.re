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

module CellType =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=90.->dp, ()));
    ();
  });

module CellAmount =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=140.->dp, ()));
    ();
  });

module CellFee =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=86.->dp, ()));
    ();
  });

module CellAddress =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=180.->dp, ()));
    ();
  });

module CellStatus =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=100.->dp, ()));
    ();
  });

module CellDate =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=180.->dp, ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style =
      Style.(
        style(~flexBasis=30.->dp, ~minWidth=30.->dp, ~alignItems=`center, ())
      );
    ();
  });

let styles =
  Style.(
    StyleSheet.create({
      "image":
        style(~marginLeft=10.->dp, ~width=19.->dp, ~height=19.->dp, ()),
    })
  );

let getContactOrRaw = OperationUtils.getContactOrRaw;
let rawUnknownAddress = OperationUtils.rawUnknownAddress;

let status = (operation: Operation.t, currentLevel, config: ConfigContext.env) => {
  let (txt, colorStyle) =
    switch (operation.status) {
    | Mempool => (I18n.state_mempool, Some(`negative))
    | Chain =>
      let minConfirmations = config.confirmations;
      let currentConfirmations = currentLevel - operation.level;
      currentConfirmations > minConfirmations
        ? (I18n.state_confirmed, None)
        : (
          I18n.state_levels(currentConfirmations, minConfirmations),
          Some(`negative),
        );
    };

  <Typography.Body1 ?colorStyle> txt->React.string </Typography.Body1>;
};

let memo = component =>
  React.memoCustomCompareProps(
    component,
    (prevPros, nextProps) => {
      let currentConfirmations =
        prevPros##currentLevel - prevPros##operation.Operation.level;

      currentConfirmations > 5 // What is that?
        ? prevPros##operation == nextProps##operation
        : prevPros##operation ==
          nextProps##operation
          &&
          prevPros##currentLevel ==
          nextProps##currentLevel;
    },
  );

module AddToken = {
  [@react.component]
  let make = (~address, ~kind: TokenRepr.kind, ~op: Operation.t, ~tokens) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();
    let closeAction = () => {
      closeAction();
    };

    let apiVersion: option(Network.apiVersion) = StoreContext.useApiVersion();

    let chain = {
      apiVersion
      ->Option.map(v => v.chain)
      ->Option.getWithDefault(Network.unsafeChainId(""));
    };

    let tooltip = (
      "add_token_from_op" ++ Operation.(op->uniqueId->uniqueIdToString),
      I18n.Tooltip.add_token,
    );
    let onPress = _ => openAction();

    <>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView
          action=`Add
          chain
          address
          kind
          tokens
          cacheOnlyNFT=true
          closeAction
        />
      </ModalAction>
      <IconButton
        icon=Icons.AddToken.build
        iconSizeRatio={5. /. 7.}
        onPress
        tooltip
      />
    </>;
  };
};

module UnknownTokenAmount = {
  [@react.component]
  let make = (~amount, ~sign, ~address: PublicKeyHash.t, ~kind, ~tokens, ~op) => {
    let tooltip = (
      "unknown_token" ++ Operation.(op->uniqueId->uniqueIdToString),
      I18n.Tooltip.unregistered_token_transaction,
    );
    <View style=OperationUtils.styles##rawAddressContainer>
      <Text>
        {Format.asprintf("%s %s", sign, amount->TokenRepr.Unit.toNatString)
         ->React.string}
      </Text>
      <IconButton
        icon=Icons.QuestionMark.build
        size=19.
        iconSizeRatio=1.
        tooltip
        disabled=true
        style=Style.(style(~borderRadius=0., ~marginLeft="4px", ()))
      />
      <AddToken address kind op tokens />
    </View>;
  };
};

module KnownTokenAmount = {
  [@react.component]
  let make =
      (
        ~amount,
        ~sign,
        ~token as {address, kind, symbol, decimals, _}: TokenRepr.t,
        ~registered,
        ~tokens,
        ~op,
      ) => {
    <View style=OperationUtils.styles##rawAddressContainer>
      <Text>
        {Format.asprintf(
           "%s %s %s",
           sign,
           amount->TokenRepr.Unit.toStringDecimals(decimals),
           symbol,
         )
         ->React.string}
      </Text>
      {registered ? React.null : <AddToken address kind op tokens />}
    </View>;
  };
};

module NFTAmount = {
  [@react.component]
  let make = (~amount, ~sign, ~token: TokenRepr.t) => {
    let source =
      NftElements.useNftSource(token, NftFilesManager.getThumbnailURL);
    <View style=OperationUtils.styles##rawAddressContainer>
      <Text>
        {Format.asprintf(
           "%s %s",
           sign,
           amount->TokenRepr.Unit.toStringDecimals(token.decimals),
         )
         ->React.string}
      </Text>
      {source->Option.mapDefault(<SVGIconNoImg />, source =>
         <Image style=styles##image source />
       )}
    </View>;
  };
};

let amount =
    (
      account: Account.t,
      transaction: Operation.Transaction.t,
      tokens,
      op: Operation.t,
    ) => {
  let colorStyle =
    account.address == transaction->Operation.Transaction.Accessor.destination
      ? `positive : `negative;

  let sign = colorStyle == `positive ? "+" : "-";
  <CellAmount>
    {<Typography.Body1 colorStyle>
       {switch (transaction) {
        | Tez(transaction) =>
          I18n.tez_op_amount(sign, transaction.amount->Tez.toString)
          ->React.string
        | Token(_, {amount, kind, contract}, _) =>
          let address = contract;
          let token: option((Token.t, bool)) =
            TokensLibrary.WithRegistration.getFullToken(
              tokens,
              address,
              kind->TokenRepr.kindId,
            );
          switch (token) {
          | None => <UnknownTokenAmount amount sign address kind tokens op />
          | Some((token, _)) when token->TokenRepr.isNFT =>
            <NFTAmount amount sign token />
          | Some((token, registered)) =>
            <KnownTokenAmount amount sign token registered tokens op />
          };
        }}
     </Typography.Body1>}
  </CellAmount>;
};

[@react.component]
let make =
  memo((~account: Account.t, ~operation: Operation.t, ~currentLevel) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let tokens = StoreContext.Tokens.useGetAll();
    let config = ConfigContext.useContent();
    let addToast = LogsContext.useToast();

    <Table.Row.Bordered>
      {switch (operation.payload) {
       | Reveal(_reveal) =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.operation_reveal->React.string
             </Typography.Body1>
           </CellType>
           <CellAmount />
           <CellFee>
             <Typography.Body1>
               {I18n.tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress />
           <CellAddress />
         </>
       | Transaction(Token(common, _, _) as transaction)
       | Transaction(Tez(common) as transaction) =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.operation_transaction->React.string
             </Typography.Body1>
           </CellType>
           {amount(account, transaction, tokens, operation)}
           <CellFee>
             <Typography.Body1>
               {I18n.tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress>
             {operation.source
              ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
              ->Option.mapWithDefault(
                  rawUnknownAddress(operation.source), alias =>
                  <Typography.Body1 numberOfLines=1>
                    alias->React.string
                  </Typography.Body1>
                )}
           </CellAddress>
           <CellAddress>
             {getContactOrRaw(aliases, tokens, common.destination)}
           </CellAddress>
         </>
       | Origination(_origination) =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.operation_origination->React.string
             </Typography.Body1>
           </CellType>
           <CellAmount />
           <CellFee />
           <CellAddress />
           <CellAddress />
           <View />
         </>
       | Delegation(delegation) =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.operation_delegation->React.string
             </Typography.Body1>
           </CellType>
           <CellAmount />
           <CellFee>
             <Typography.Body1>
               {I18n.tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress>
             {getContactOrRaw(aliases, tokens, operation.source)}
           </CellAddress>
           {delegation.delegate
            ->Option.mapWithDefault(
                <CellAddress>
                  <Typography.Body1 numberOfLines=1>
                    I18n.delegation_removal->React.string
                  </Typography.Body1>
                </CellAddress>,
                d =>
                <CellAddress>
                  {getContactOrRaw(aliases, tokens, d)}
                </CellAddress>
              )}
         </>
       | Unknown =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.unknown_operation->ReasonReact.string
             </Typography.Body1>
           </CellType>
           <CellAmount />
           <CellFee />
           <CellAddress />
         </>
       }}
      <CellDate>
        <Typography.Body1>
          {operation.timestamp->DateFns.format("P pp")->React.string}
        </Typography.Body1>
      </CellDate>
      <CellStatus> {status(operation, currentLevel, config)} </CellStatus>
      <CellAction>
        <IconButton
          size=34.
          icon=Icons.OpenExternal.build
          tooltip=(
            "open_in_explorer"
            ++ Operation.(operation->uniqueId->uniqueIdToString),
            I18n.Tooltip.open_in_explorer,
          )
          onPress={_ => {
            switch (Network.externalExplorer(config.network.chain)) {
            | Ok(url) => System.openExternal(url ++ operation.hash)
            | Error(err) => addToast(Logs.error(~origin=Operation, err))
            }
          }}
        />
      </CellAction>
    </Table.Row.Bordered>;
  });
