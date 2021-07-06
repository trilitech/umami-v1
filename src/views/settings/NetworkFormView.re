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

module StateLenses = [%lenses
  type state = {
    name: string,
    node: string,
    mezos: string,
  }
];
module NetworkCreateForm = ReForm.Make(StateLenses);

type action =
  | Create
  | Edit(Network.network);

let nameExistsCheck =
    (name: string, customNetworks: list(Network.network))
    : ReSchema.fieldState =>
  List.some(customNetworks, n => n.name === name)
    ? Error(I18n.form_input_error#name_already_taken(name)) : Valid;

let isEditMode =
  fun
  | Edit(_) => true
  | _ => false;

let styles =
  Style.(
    StyleSheet.create({
      "buttonsRow":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "title": style(~marginBottom=6.->dp, ~textAlign=`center, ()),
      "overline": style(~marginBottom=24.->dp, ~textAlign=`center, ()),
    })
  );

[@react.component]
let make = (~initNode=?, ~initMezos=?, ~action: action, ~closeAction) => {
  let writeConf = ConfigContext.useWrite();

  let customNetworks = ConfigContext.useContent().customNetworks;

  let addToast = LogsContext.useToast();

  let addCustomNetwork = (network: Network.network) => {
    writeConf(c =>
      {...c, customNetworks: List.add(c.customNetworks, network)}
    );
  };

  let editCustomNetwork =
      (network: Network.network, newNetwork: Network.network) => {
    writeConf(c =>
      {
        ...c,
        customNetworks:
          List.map(c.customNetworks, n => n === network ? newNetwork : n),
      }
    );
  };

  let form: NetworkCreateForm.api =
    NetworkCreateForm.use(
      ~schema={
        NetworkCreateForm.Validation.(
          Schema(
            nonEmpty(Name)
            + nonEmpty(Node)
            + nonEmpty(Mezos)
            + custom(
                values => nameExistsCheck(values.name, customNetworks),
                Name,
              ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          switch (action) {
          | Create =>
            Network.checkConfiguration(state.values.mezos, state.values.node)
            ->Future.mapOk(((_, chain)) =>
                addCustomNetwork({
                  name: state.values.name,
                  chain,
                  explorer: state.values.mezos,
                  endpoint: state.values.node,
                })
              )
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addToast, Logs.Account, _ =>
                I18n.t#custom_network_created
              )
            ->ignore
          | Edit(network) =>
            Network.checkConfiguration(state.values.mezos, state.values.node)
            ->Future.mapOk(((_, chain)) =>
                editCustomNetwork(
                  network,
                  {
                    name: state.values.name,
                    chain,
                    explorer: state.values.mezos,
                    endpoint: state.values.node,
                  },
                )
              )
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addToast, Logs.Account, _ =>
                I18n.t#custom_network_updated
              )
            ->ignore
          };

          None;
        },
      ~initialState=
        switch (action) {
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
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.headerWithoutMarginBottom>
      (
        switch (action) {
        | Create => I18n.title#add_custom_network
        | Edit(_) => I18n.title#update_custom_network
        }
      )
      ->React.string
    </Typography.Headline>
    {<Typography.Overline3 style=styles##overline>
       I18n.expl#custom_network->React.string
     </Typography.Overline3>
     ->ReactUtils.onlyWhen(action === Create)}
    <>
      <FormGroupTextInput
        label=I18n.label#custom_network_name
        value={form.values.name}
        placeholder=I18n.input_placeholder#custom_network_name
        handleChange={form.handleChange(Name)}
        error={form.getFieldError(Field(Name))}
      />
      <FormGroupTextInput
        label=I18n.label#custom_network_node_url
        value={form.values.node}
        placeholder=I18n.input_placeholder#custom_network_node_url
        handleChange={form.handleChange(Node)}
        error={form.getFieldError(Field(Node))}
        disabled=?{
          switch (action) {
          | Create => None
          | Edit(_) => None
          }
        }
        clearButton=true
      />
      <FormGroupTextInput
        label=I18n.label#custom_network_mezos_url
        value={form.values.mezos}
        placeholder=I18n.input_placeholder#custom_network_mezos_url
        handleChange={form.handleChange(Mezos)}
        error={form.getFieldError(Field(Mezos))}
        disabled=?{
          switch (action) {
          | Create => None
          | Edit(_) => None
          }
        }
        clearButton=true
      />
    </>
    <Buttons.SubmitPrimary
      text={
        switch (action) {
        | Create => I18n.btn#add
        | Edit(_) => I18n.btn#update
        }
      }
      onPress=onSubmit
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
