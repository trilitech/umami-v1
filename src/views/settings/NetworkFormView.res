/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

module StateLenses = %lenses(
  type state = {
    name: string,
    node: string,
    mezos: string,
  }
)
module NetworkCreateForm = ReForm.Make(StateLenses)

type action =
  | Create
  | Edit(Network.network)

let rsf = (err: Errors.t, raiseSubmitFailed) =>
  switch err {
  | Network.API(_) => raiseSubmitFailed(Some("APIError"))
  | Network.Node(_) => raiseSubmitFailed(Some("NodeError"))
  | Network.ChainInconsistency(_, _) => raiseSubmitFailed(Some("ChainInconsistency"))
  | Network.APIAndNodeError(_, _) => raiseSubmitFailed(Some("TwoErrors"))
  | Network.UnknownChainId(_) => raiseSubmitFailed(Some("UnknownChainId"))
  | _ => ()
  }

let nodeErrorFilter = x =>
  switch x {
  | Some("NodeError") => Some(I18n.Form_input_error.node_not_available)
  | Some("TwoErrors") => Some(I18n.Form_input_error.node_not_available)
  | Some("Inconsistency") => Some(I18n.Form_input_error.different_chains)
  | _ => None
  }

let mezosErrorFilter = x =>
  switch x {
  | Some("APIError") => Some(I18n.Form_input_error.api_not_available)
  | Some("TwoErrors") => Some(I18n.Form_input_error.api_not_available)
  | Some("Inconsistency") => Some(I18n.Form_input_error.different_chains)
  | _ => None
  }

let styles = {
  open Style
  StyleSheet.create({
    "buttonsRow": style(
      ~flexDirection=#row,
      ~alignItems=#center,
      ~justifyContent=#spaceBetween,
      (),
    ),
    "title": style(~marginBottom=6.->dp, ~textAlign=#center, ()),
    "overline": style(~marginBottom=24.->dp, ~textAlign=#center, ()),
  })
}

@react.component
let make = (~initNode=?, ~initMezos=?, ~action: action, ~closeAction) => {
  let writeConf = ConfigContext.useWrite()

  let (loading, setLoading) = React.useState(_ => false)

  let customNetworks = ConfigContext.useFile().customNetworks

  let addToast = LogsContext.useToast()

  let addCustomNetwork = (network: Network.network) =>
    writeConf(c => {...c, customNetworks: List.add(c.customNetworks, network)})

  let editCustomNetwork = (network: Network.network, newNetwork: Network.network) =>
    writeConf(c => {
      ...c,
      customNetworks: List.map(c.customNetworks, n => n === network ? newNetwork : n),
      network: c.network == Some(#Custom(network.name))
        ? Some(#Custom(newNetwork.name))
        : c.network,
    })

  let nameExistsCheck = (
    name: string,
    customNetworks: list<Network.network>,
  ): ReSchema.fieldState =>
    List.some(Network.nativeChains->List.map(fst)->List.map(Network.getDisplayedName), n =>
      n == name
    ) ||
    (List.some(customNetworks, n => n.name == name) &&
    switch action {
    | Create => true
    | Edit(network) => name != network.name
    })
      ? Error(I18n.Form_input_error.name_already_taken(name))
      : Valid

  let form: NetworkCreateForm.api = NetworkCreateForm.use(
    ~schema={
      open NetworkCreateForm.Validation
      Schema(
        nonEmpty(Name) +
        nonEmpty(Node) +
        nonEmpty(Mezos) +
        custom(values => nameExistsCheck(values.name, customNetworks), Name),
      )
    },
    ~onSubmit=({state, raiseSubmitFailed}) => {
      let networkChanged = (network: Network.network) =>
        network.explorer != state.values.mezos || network.endpoint != state.values.node

      let log = switch action {
      | Create => I18n.custom_network_created
      | Edit(_) => I18n.custom_network_updated
      }

      let checkConfig = () => {
        setLoading(_ => true)
        Network.checkConfiguration(state.values.mezos, state.values.node)
        ->Promise.mapOk(snd)
        ->Promise.tap(_ => setLoading(_ => false))
      }

      let chain = switch action {
      | Create => checkConfig()
      | Edit(network) => networkChanged(network) ? checkConfig() : network.chain->Ok->Promise.value
      }

      chain
      ->Promise.mapOk(chain => {
        let newNetwork = {
          open Network
          {
            name: state.values.name,
            chain: chain,
            explorer: state.values.mezos,
            endpoint: state.values.node,
          }
        }

        switch action {
        | Create => addCustomNetwork(newNetwork)
        | Edit(network) => editCustomNetwork(network, newNetwork)
        }
      })
      ->ApiRequest.logOk(addToast, Logs.Account, _ => log)
      ->Promise.get(x =>
        switch x {
        | Ok() => closeAction()
        | Error(e) => e->rsf(raiseSubmitFailed)
        }
      )

      None
    },
    ~initialState=switch action {
    | Create => {
        name: "",
        node: initNode->Option.getWithDefault(""),
        mezos: initMezos->Option.getWithDefault(""),
      }
    | Edit(network) => {
        name: network.name,
        node: network.endpoint,
        mezos: network.explorer,
      }
    },
    ~i18n=FormUtils.i18n,
    (),
  )

  let onSubmit = _ => form.submit()

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <Typography.Headline style=FormStyles.headerWithoutMarginBottom>
      {switch action {
      | Create => I18n.Title.add_custom_network
      | Edit(_) => I18n.Title.update_custom_network
      }->React.string}
    </Typography.Headline>
    {<Typography.Overline3 style={styles["overline"]}>
      {I18n.Expl.custom_network->React.string}
    </Typography.Overline3>->ReactUtils.onlyWhen(action === Create)}
    {<>
      <FormGroupTextInput
        label=I18n.Label.custom_network_name
        value=form.values.name
        placeholder=I18n.Input_placeholder.custom_network_name
        handleChange={form.handleChange(Name)}
        error={form.getFieldError(Field(Name))}
        onSubmitEditing=onSubmit
      />
      <FormGroupTextInput
        label={I18n.Label.custom_network_node_url(false)}
        value=form.values.node
        placeholder=I18n.Input_placeholder.custom_network_node_url
        handleChange={form.handleChange(Node)}
        error={list{
          form.formState->FormUtils.getFormStateError->nodeErrorFilter,
          form.getFieldError(Field(Node)),
        }->Option.firstSome}
        onSubmitEditing=onSubmit
        disabled=?{switch action {
        | Create => None
        | Edit(_) => None
        }}
        clearButton=true
      />
      <FormGroupTextInput
        label=I18n.Label.custom_network_mezos_url
        value=form.values.mezos
        placeholder=I18n.Input_placeholder.custom_network_mezos_url
        handleChange={form.handleChange(Mezos)}
        error={list{
          form.formState->FormUtils.getFormStateError->mezosErrorFilter,
          form.getFieldError(Field(Mezos)),
        }->Option.firstSome}
        onSubmitEditing=onSubmit
        disabled=?{switch action {
        | Create => None
        | Edit(_) => None
        }}
        clearButton=true
      />
    </>}
    <Buttons.SubmitPrimary
      text={switch action {
      | Create => I18n.Btn.add
      | Edit(_) => I18n.Btn.update
      }}
      onPress=onSubmit
      style=FormStyles.formSubmit
      loading
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>
}