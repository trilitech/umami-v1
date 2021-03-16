open ReactNative;

module StateLenses = [%lenses
  type state = {
    name: string,
    secret: string,
  }
];
module AccountCreateForm = ReForm.Make(StateLenses);

type action =
  | Create
  | Edit(Account.t);

module Generic = {
  [@react.component]
  let make =
      (
        ~init,
        ~buttonText,
        ~action: (~name: string, ~secretIndex: int) => unit,
        ~request,
        ~secret: option(Secret.t)=?,
        ~hideSecretSelector=false,
      ) => {
    let form: AccountCreateForm.api =
      AccountCreateForm.use(
        ~schema={
          AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
        },
        ~onSubmit=
          ({state}) => {
            action(
              ~name=state.values.name,
              ~secretIndex=
                state.values.secret->Js.Float.fromString->int_of_float,
            );
            None;
          },
        ~initialState={
          name: init,
          secret:
            secret->Option.mapWithDefault("", secret =>
              secret.index->string_of_int
            ),
        },
        ~i18n=FormUtils.i18n,
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    let loading = request->ApiRequest.isLoading;

    let formFieldsAreValids =
      FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

    <>
      {hideSecretSelector
         ? React.null
         : <FormGroupSecretSelector
             label=I18n.label#account_secret
             value={form.values.secret}
             handleChange={form.handleChange(Secret)}
             error={form.getFieldError(Field(Secret))}
             disabled={secret->Option.isSome}
           />}
      <FormGroupTextInput
        label=I18n.label#account_create_name
        value={form.values.name}
        handleChange={form.handleChange(Name)}
        error={form.getFieldError(Field(Name))}
      />
      <Buttons.SubmitPrimary
        text=buttonText
        onPress=onSubmit
        loading
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </>;
  };
};

module Update = {
  [@react.component]
  let make = (~closeAction, ~account: Account.t) => {
    let (updateAccountRequest, updateAccount) =
      StoreContext.Accounts.useUpdate();

    let addLog = LogsContext.useAdd();

    let action = (~name as new_name, ~secretIndex as _s) => {
      updateAccount({old_name: account.alias, new_name})
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_updated
        )
      ->Future.tapOk(() => closeAction())
      ->ignore;
    };

    <ModalFormView closing={ModalFormView.Close(closeAction)}>
      <Typography.Headline style=FormStyles.header>
        I18n.title#account_update->React.string
      </Typography.Headline>
      <Generic
        init={account.alias}
        buttonText=I18n.btn#update
        request=updateAccountRequest
        action
        hideSecretSelector=true
      />
    </ModalFormView>;
  };
};

module Create = {
  module PasswordView = {
    module StateLenses = [%lenses type state = {password: string}];

    module PasswordForm = ReForm.Make(StateLenses);

    [@react.component]
    let make = (~request, ~action) => {
      let form: PasswordForm.api =
        PasswordForm.use(
          ~schema={
            PasswordForm.Validation.(Schema(nonEmpty(Password)));
          },
          ~onSubmit=
            ({state}) => {
              action(~password=state.values.password);
              None;
            },
          ~initialState={password: ""},
          ~i18n=FormUtils.i18n,
          (),
        );

      let onSubmit = _ => {
        form.submit();
      };

      let loading = request->ApiRequest.isLoading;

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <FormGroupTextInput
          label=I18n.label#password
          value={form.values.password}
          handleChange={form.handleChange(Password)}
          error={form.getFieldError(Field(Password))}
          textContentType=`password
          secureTextEntry=true
          onSubmitEditing={_event => {form.submit()}}
        />
        <View style=FormStyles.verticalFormAction>
          <Buttons.SubmitPrimary
            text=I18n.btn#confirm
            onPress=onSubmit
            loading
            disabledLook={!formFieldsAreValids}
          />
        </View>
      </>;
    };
  };

  [@react.component]
  let make = (~closeAction, ~secret: option(Secret.t)=?) => {
    let (createDeriveRequest, deriveAccount) =
      StoreContext.Secrets.useDerive();

    let addLog = LogsContext.useAdd();

    let (formValues, setFormValues) = React.useState(_ => None);

    let action = (~name, ~secretIndex, ~password) => {
      deriveAccount({name, index: secretIndex, password})
      ->Future.mapOk(_ => ())
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_created
        )
      ->Future.tapOk(() => closeAction())
      ->ignore;
    };

    <ModalFormView closing={ModalFormView.Close(closeAction)}>
      <Typography.Headline style=FormStyles.header>
        I18n.title#account_create->React.string
      </Typography.Headline>
      {switch (formValues) {
       | None =>
         <Generic
           init=""
           buttonText=I18n.btn#add
           request=createDeriveRequest
           action={(~name, ~secretIndex) =>
             setFormValues(_ => Some((name, secretIndex)))
           }
           ?secret
         />
       | Some((name, secretIndex)) =>
         <PasswordView
           request=createDeriveRequest
           action={action(~name, ~secretIndex)}
         />
       }}
    </ModalFormView>;
  };
};
