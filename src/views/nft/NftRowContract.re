open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "address": style(~marginLeft=auto, ()),
      "iconButton": style(~marginLeft=4.->dp, ()),
      "clipButton": style(~marginLeft=10.->dp, ()),
      "containerStyle": style(~paddingLeft=10.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~contract: TokensLibrary.WithBalance.contract,
      ~account,
      ~selected,
      ~setSelected,
      ~hidden,
      ~setHidden,
    ) => {
  let config = ConfigContext.useContent();

  let (expanded, setExpanded) = React.useState(_ => true);
  let data = {
    contract.tokens
    ->Map.Int.map(
        fun
        | (Partial(_, _, _), _) => React.null
        | (Full(nft), _) =>
          <NftRowToken
            account
            address={contract.address}
            nft
            selected
            setSelected
            hidden
            setHidden
          />,
      )
    |> Map.Int.valuesToArray
    |> React.array;
  };
  let addToast = LogsContext.useToast();
  let theme = ThemeContext.useTheme();
  let header =
    <View
      style=Style.(
        array([|
          styles##containerStyle,
          NftRowToken.styles##flex_row,
          style(~borderColor=theme.colors.borderDisabled, ()),
        |])
      )>
      <IconButton
        icon={
          expanded
            ? (~color=?, ~style as _=?) =>
                Icons.ChevronDown.build(
                  ~color?,
                  ~style=Buttons.styles##chevronUp,
                )
            : Icons.ChevronDown.build
        }
        iconSizeRatio={5. /. 7.}
        onPress={_ => setExpanded(expanded => !expanded)}
        style=styles##iconButton
      />
      <Typography.Notice style=styles##iconButton>
        {contract.name
         ->Option.getWithDefault((contract.address :> string))
         ->React.string}
      </Typography.Notice>
      <Typography.Address style=styles##address>
        (contract.address :> string)->React.string
      </Typography.Address>
      <ClipboardButton
        iconSizeRatio={5. /. 7.}
        style=styles##clipButton
        copied=I18n.label#contract_address
        addToast
        data=(contract.address :> string)
      />
      <IconButton
        size=34.
        icon=Icons.OpenExternal.build
        tooltip=(
          "open_in_explorer" ++ (contract.address :> string),
          I18n.tooltip#open_in_explorer,
        )
        onPress={_ => {
          switch (Network.externalExplorer(config.network.chain)) {
          | Ok(url) =>
            System.openExternal(url ++ (contract.address :> string))
          | Error(err) => addToast(Logs.error(~origin=Operation, err))
          }
        }}
      />
    </View>;

  <View> <Accordion header data expanded /> </View>;
};
