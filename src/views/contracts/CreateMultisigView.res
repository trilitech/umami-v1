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

let styles = {
  open Style
  StyleSheet.create({
    "suffix": style(~marginLeft=8.->dp, ()),
    "row": style(~flexDirection=#row, ~alignContent=#center, ~alignItems=#center, ()),
    "selectorItem": style(~lineHeight=34., ~marginHorizontal=16.->dp, ()),
    "selectorButtonContent": style(
      ~width=201.->dp,
      ~height=44.->dp,
      ~display=#flex,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~justifyContent=#spaceBetween,
      (),
    ),
  })
}

module StepView = {
  let styles = {
    open Style
    StyleSheet.create({
      "body": style(
        ~maxWidth=939.->dp,
        ~minHeight=auto,
        ~borderRadius=8.,
        ~marginTop=8.->dp,
        ~marginBottom=8.->dp,
        ~flexDirection=#row,
        ~padding=16.->dp,
        (),
      ),
      "content": style(~flexBasis=0.->dp, ~flexGrow=1., ~marginRight=40.->dp, ()),
      "title": style(~marginBottom=4.->dp, ()),
    })
  }

  module Badge = {
    let styles = {
      open Style
      StyleSheet.create({
        "badge": StyleSheet.flatten([
          FormStyles.square(24.),
          Typography.Base.styles["text"],
          style(
            ~marginTop=-2.->dp,
            ~marginRight=16.->dp,
            ~justifyContent=#center,
            ~alignItems=#center,
            ~alignSelf=#flexStart,
            ~borderRadius=12.,
            ~fontSize=16.,
            ~fontWeight=#bold,
            (),
          ),
        ]),
      })
    }

    @react.component
    let make = (~step, ~disabled=false) => {
      let theme = ThemeContext.useTheme()

      <View
        style={
          open Style
          array([
            styles["badge"],
            style(
              ~color=theme.colors.background,
              ~backgroundColor=disabled ? theme.colors.iconDisabled : theme.colors.iconMaxEmphasis,
              (),
            ),
          ])
        }>
        {<> {step->Int.toString->React.string} </>}
      </View>
    }
  }

  module Ok = {
    @react.component
    let make = () => {
      let theme = ThemeContext.useTheme()

      <View
        style={
          open Style
          StyleSheet.flatten([
            FormStyles.square(20.),
            style(
              ~backgroundColor=theme.colors.textPositive,
              ~marginLeft=2.->dp,
              ~marginRight=18.->dp,
              ~justifyContent=#center,
              ~alignItems=#center,
              ~alignSelf=#flexStart,
              ~borderRadius=10.,
              (),
            ),
          ])
        }>
        <Icons.Ok.I size=16. color=theme.colors.elevatedBackground />
      </View>
    }
  }

  @react.component
  let make = (~step, ~currentStep, ~title, ~children) => {
    let theme = ThemeContext.useTheme()
    let disabled = step > currentStep
    <View
      style={
        open Style
        array([styles["body"], style(~backgroundColor=theme.colors.elevatedBackground, ())])
      }>
      {step < currentStep ? <Ok /> : <Badge step disabled />}
      <View style={styles["content"]}>
        <Typography.Subtitle2
          colorStyle={disabled ? #disabled : #highEmphasis} style={styles["title"]}>
          {title->React.string}
        </Typography.Subtitle2>
        {ReactUtils.onlyWhen(<> children </>, step == currentStep)}
      </View>
    </View>
  }
}

module Form = {
  module StateLenses = %lenses(
    type state = {
      name: string,
      owners: array<FormUtils.Alias.any>,
      threshold: ReBigNumber.t,
    }
  )

  include ReForm.Make(StateLenses)

  let ownerAddresses = (t: state) =>
    t.values.owners->Array.keepMap(owner =>
      switch owner {
      | Valid(t) => Some(t->FormUtils.Alias.address)
      | _ => None
      }
    )

  let originationOperation = (t: state, ~source: Account.t) =>
    Multisig.API.origination(
      ~source,
      ~ownerAddresses=t->ownerAddresses,
      ~threshold=t.values.threshold,
    )

  let toMultisig = (t: state, ~address, ~chain) => {
    open Multisig
    {
      address: address,
      alias: t.values.name,
      balance: ReBigNumber.zero,
      chain: chain,
      signers: t->ownerAddresses,
      threshold: t.values.threshold,
    }
  }
}

let validateOwners: array<FormUtils.Alias.any> => ReSchema.fieldState = owners => {
  let childStates =
    owners
    ->Array.mapWithIndex((index, owner) =>
      switch owner {
      | AnyString(_) =>
        Some({ReSchema.error: I18n.Form_input_error.invalid_contract, index: index, name: ""})

      | Temp(_, Pending | NotAsked) => Some({ReSchema.error: "", index: index, name: ""})
      | Temp(_, Error(s)) => Some({ReSchema.error: s, index: index, name: ""})
      | Valid(Alias(_)) => None
      | Valid(Address(_)) => None
      }
    )
    ->Array.keepMap(state => state)
  childStates->Array.length == 0 ? Valid : childStates->NestedErrors
}

let onlyValidOwners = array =>
  array->Array.keepMap(owner =>
    switch owner {
    | FormUtils.Alias.Valid(alias) => Some(alias)
    | _ => None
    }
  )

let submitOnEnterKeyPress = (onSubmit, event) =>
  if (event->TextInput.KeyPressEvent.nativeEvent)["key"] === "Enter" {
    TextInput.KeyPressEvent.preventDefault(event)
    onSubmit()
  }

module CreateStep_Name = {
  @react.component
  let make = (~form: Form.api, ~onSubmit) => {
    <>
      {I18n.Title.name_contract->Typography.overline1(~style=FormStyles.headerWithoutMarginBottom)}
      {I18n.Expl.name_multisig->Typography.body1(~style=FormStyles.textContent)}
      <FormGroupTextInput
        label=I18n.Label.add_contract_name
        value=form.values.name
        handleChange={form.handleChange(Name)}
        placeholder=I18n.Input_placeholder.add_contract_name
        error={list{
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Name)),
        }->Option.firstSome}
        onKeyPress={submitOnEnterKeyPress(onSubmit)}
      />
      <Buttons.SubmitPrimary
        text=I18n.Btn.continue
        onPress={_ => onSubmit()}
        style=FormStyles.formSubmit
        disabled={form.getFieldState(Field(Name)) != Valid}
      />
    </>
  }
}

module CreateStep_OwnerThreshold = {
  let keep: (array<Umami.FormUtils.Alias.any>, Umami.Alias.t) => bool = (formValues, a) => {
    let addressesInForm = formValues->Array.keepMap(owner =>
      switch owner {
      | FormUtils.Alias.Valid(t) => Some(t->FormUtils.Alias.address)
      | _ => None
      }
    )

    // Only keep aliases that are not already added in form
    !(addressesInForm->Array.some(f => f === a.address))
  }

  let removeOwner = (form: Form.api, i) => {
    if (
      form.values.threshold->ReBigNumber.isGreaterThanOrEqualTo(
        ReBigNumber.fromInt(form.values.owners->Array.length),
      )
    ) {
      form.handleChangeWithCallback(Threshold, threshold =>
        threshold->ReBigNumber.minus(ReBigNumber.fromInt(1))
      )
    }
    form.handleChangeWithCallback(Owners, owners => owners->Array.keepWithIndex((_, j) => i != j))
  }

  let addOwner = (form: Form.api, i, newOwner) =>
    form.handleChangeWithCallback(Owners, owners =>
      owners->Array.mapWithIndex((j, owner) => i == j ? newOwner : owner)
    )

  let renderItem = item => {item->Int.toString->Typography.body1(~style=styles["selectorItem"])}

  module OwnerSelector = {
    @react.component
    let make = (~form: Form.api, ~i, ~value) => {
      let key = i->Int.toString
      let existingOwners = form.values.owners->Array.removeAtIndex(i)
      <FormGroupContactSelector
        keyPopover={"formGroupContactSelector" ++ key}
        label={i == 0 ? I18n.Label.owners : ""}
        keep={keep(existingOwners)}
        value
        onRemove=?{i != 0 ? {() => removeOwner(form, i)}->Some : None}
        onChange={addOwner(form, i)}
        error={form.getNestedFieldError(Field(Owners), i)}
      />
    }
  }

  @react.component
  let make = (~form: Form.api, ~onSubmit) => {
    let theme = ThemeContext.useTheme()
    <>
      {I18n.Title.set_owners_and_threshold->Typography.overline1(
        ~style=FormStyles.headerWithoutMarginBottom,
      )}
      {I18n.Expl.set_multisig_owners->Typography.body1(~style=FormStyles.textContent)}
      {form.values.owners
      ->Array.mapWithIndex((i, value) => <OwnerSelector form i value />)
      ->React.array}
      <ButtonAction
        style={
          open Style
          style(~alignSelf=#flexStart, ~marginTop=-12.->dp, ~marginBottom=28.->dp, ())
        }
        onPress={_ => form.arrayPush(Owners, FormUtils.Alias.AnyString(""))}
        text=I18n.Btn.add_another_owner
        icon=Icons.Add.build
        primary=true
      />
      {I18n.Expl.set_multisig_threshold->Typography.body1(~style=FormStyles.marginBottom16)}
      {
        let validOwnersNumber = switch onlyValidOwners(form.values.owners)->Array.length {
        | 0 => 1
        | x => x
        }
        <View
          style={
            open Style
            StyleSheet.flatten([styles["row"], style(~marginBottom=12.->dp, ())])
          }>
          <Selector
            globalStyle=#button
            chevron={<Icons.ChevronDown
              size=22. color=theme.colors.iconHighEmphasis style={Selector.styles["icon"]}
            />}
            innerStyle={styles["selectorButtonContent"]}
            items={Array.range(1, validOwnersNumber)}
            getItemKey={item => item->Int.toString}
            renderItem
            selectedValueKey={form.values.threshold->ReBigNumber.toString}
            onValueChange={item => form.handleChange(Threshold, ReBigNumber.fromInt(item))}
            renderButton={(selectedItem, _) => {
              selectedItem->Option.mapWithDefault(React.null, item => renderItem(item))
            }}
            keyPopover="Selector"
            backgroundColor=theme.colors.elevatedBackground
          />
          {I18n.Expl.out_of(validOwnersNumber->Int.toString)->Typography.body1(
            ~style=styles["suffix"],
          )}
        </View>
      }
      <Buttons.SubmitPrimary
        text=I18n.Btn.continue
        onPress={_ => onSubmit()}
        style=FormStyles.formSubmit
        disabled={form.values.owners->Array.some(x =>
          switch x {
          | Valid(_) => false
          | _ => true
          }
        )}
      />
    </>
  }
}

module CreateStep_Review = {
  @react.component
  let make = (~form: Form.api, ~onSubmit) => {
    let owners = form.values.owners->Array.keepMap(owner =>
      switch owner {
      | FormUtils.Alias.Valid(alias) => Some(alias)
      | _ => None
      }
    )
    <>
      {I18n.Title.review_and_submit->Typography.overline1(
        ~style=FormStyles.headerWithoutMarginBottom,
      )}
      {I18n.Expl.review_multisig->Typography.body1(~style=FormStyles.textAlignCenter)}
      <ContractDetailsView.Multisig.Name name=form.values.name />
      <ContractDetailsView.Multisig.Owners
        owners={Array.map(owners, FormUtils.Alias.address)} shrinkedAddressDisplay=true
      />
      <ContractDetailsView.Multisig.Threshold
        threshold={form.values.threshold} owners={owners->Array.length}
      />
      <Buttons.SubmitPrimary
        text=I18n.Btn.submit onPress={_ => onSubmit()} style=FormStyles.formSubmit
      />
    </>
  }
}

type step =
  | CreateStep_Name
  | CreateStep_OwnerThreshold
  | CreateStep_Review
  | SourceStep(Form.state, option<string> => unit)
  | SigningStep(
      Form.state,
      option<string> => unit,
      array<Protocol.manager>,
      Protocol.Simulation.results,
    )
  | SubmittedStep(PublicKeyHash.t, string) // (originator, operation hash)

type Errors.t +=
  | AddressNotFound

module SignView = {
  @react.component
  let make = (
    ~account,
    ~state,
    ~signOpStep,
    ~dryRun,
    ~operations,
    ~name,
    ~loading,
    ~onOperationAndSigningIntent,
  ) => {
    let theme = ThemeContext.useTheme()
    <SignOperationView
      signer=account
      dryRun
      signOpStep
      state
      operations
      sendOperation={onOperationAndSigningIntent}
      loading
      icon={_ => Some(
        Icons.Key.build(~style=?None, ~size=20., ~color=theme.colors.iconMediumEmphasis),
      )}
      name
    />
  }
}

let getAddress = (result: ReTaquito.Toolkit.Operation.result) => {
  let contracts = result.results->Array.keepMap(result =>
    try {
      switch result->ReTaquitoTypes.Operation.classifiy {
      | Origination(result) => result.metadata.operation_result.originated_contracts[0]
      | exception _ => None
      }
    } catch {
    | _ => None
    }
  )
  contracts[0]
  ->Promise.fromOption(~error=AddressNotFound)
  ->Promise.mapOk(s => s->PublicKeyHash.build->Result.getExn)
}

@react.component
let make = (~closeAction) => {
  let setDefaultAccount = StoreContext.SelectedAccount.useSet()
  let init = StoreContext.SelectedAccount.useGetImplicit()->Option.getExn
  let (account, setAccount) = React.useState(_ => init)
  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()
  let (modalStep, setModalStep) = React.useState(_ => CreateStep_Name)
  let form = Form.use(
    ~schema={
      open Form.Validation
      Schema(
        nonEmpty(Name) +
        custom(values => validateOwners(values.owners), Owners) +
        custom(_ => Valid, Threshold),
      )
    },
    ~onSubmit=({state, raiseSubmitFailed}) => {
      setModalStep(_ => SourceStep(state, raiseSubmitFailed))
      None
    },
    ~initialState={
      name: "",
      owners: [FormUtils.Alias.AnyString("")],
      threshold: ReBigNumber.fromInt(1),
    },
    ~i18n=FormUtils.i18n,
    (),
  )
  let state = React.useState(() => None)
  let (sign, setSign) as signOpStep = React.useState(() => SignOperationView.SummaryStep)
  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()
  let (multisigRequest, updateMultisig) = StoreContext.Multisig.useUpdate(true)
  let config = ConfigContext.useContent()

  let back = switch modalStep {
  | CreateStep_OwnerThreshold => Some(_ => setModalStep(_ => CreateStep_Name))
  | CreateStep_Review => Some(_ => setModalStep(_ => CreateStep_OwnerThreshold))
  | SourceStep(_, _) => Some(_ => setModalStep(_ => CreateStep_Review))
  | SigningStep(state, raiseSubmitFailed, _, _) =>
    switch sign {
    | AdvancedOptStep(_) => Some(() => setSign(_ => SummaryStep))
    | SummaryStep => Some(_ => setModalStep(_ => SourceStep(state, raiseSubmitFailed)))
    }
  | _ => None
  }

  let closing = switch modalStep {
  | SubmittedStep(_) => ModalFormView.Close(closeAction)->Some
  | SigningStep(_, _, _, _)
    if switch sign {
    | AdvancedOptStep(_) => true
    | SummaryStep => false
    } =>
    None
  | _ => Some(ModalFormView.confirm(~actionText=I18n.Btn.cancel, closeAction))
  }

  let title = switch modalStep {
  | CreateStep_Name
  | CreateStep_OwnerThreshold
  | CreateStep_Review
  | SourceStep(_) =>
    Some(I18n.Title.create_new_multisig)
  | SigningStep(_) => Some(I18n.Title.confirm_multisig_origination)
  | SubmittedStep(_) => None
  }

  let handleSource = (state, raiseSubmitFailed, source: Umami.Account.t) => {
    setAccount(_ => source)
    // Don't use account instead of source because React won't have update account already.
    let operations = [state->Form.originationOperation(~source)]
    sendOperationSimulate(source, operations)->Promise.get(x => {
      switch x {
      | Error(e) => raiseSubmitFailed(Some(e->Errors.toString))
      | Ok(dryRun) => setModalStep(_ => SigningStep(state, raiseSubmitFailed, operations, dryRun))
      }
    })
  }

  let handleOperationAndSigningIntent = (account, ~operation, signingIntent) => {
    sendOperation({
      operation: operation,
      signingIntent: signingIntent,
    })->Promise.tapOk((result: ReTaquito.Toolkit.Operation.result) => {
      setModalStep(_ => SubmittedStep(account, result.hash))
      result
      ->getAddress
      ->Promise.mapOk(address =>
        form.state
        ->Form.toMultisig(~address, ~chain=config.network.chain->Network.getChainId)
        ->updateMultisig
      )
    })
  }
  let stepToString = step =>
    switch step {
    | CreateStep_Name => "CreateStep_Name"
    | CreateStep_OwnerThreshold =>
      "CreateStep_OwnerThreshold_" ++ form.values.owners->Array.length->Int.toString
    | CreateStep_Review => "CreateStep_Review"
    | SourceStep(_) => "sourceStep"
    | SigningStep(_) => "signingstep"
    | SubmittedStep(_) => "submittedstep"
    }

  let step = i =>
    I18n.stepof(i, 3)->Typography.overline3(
      ~colorStyle=#highEmphasis,
      ~style={FormStyles.onboarding["stepPager"]},
    )

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView ?title back ?closing titleStyle=FormStyles.headerMarginBottom8>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch modalStep {
          | CreateStep_Name => <>
              {step(1)}
              <CreateStep_Name form onSubmit={() => setModalStep(_ => CreateStep_OwnerThreshold)} />
            </>
          | CreateStep_OwnerThreshold => <>
              {step(2)}
              <CreateStep_OwnerThreshold
                form onSubmit={() => setModalStep(_ => CreateStep_Review)}
              />
            </>
          | CreateStep_Review => <> {step(3)} <CreateStep_Review form onSubmit=form.submit /> </>
          | SourceStep(state, raiseSubmitFailed) =>
            <SourceSelector
              filter=PublicKeyHash.isImplicit
              source={Alias.fromAccount(account)}
              loading={operationSimulateRequest->ApiRequest.isLoading}
              onSubmit={source =>
                handleSource(state, raiseSubmitFailed, source->Alias.toAccountExn)} //FIXME
            />
          | SigningStep(_, _, operations, dryRun) =>
            <SignView
              account
              state
              signOpStep
              dryRun
              operations
              name={_ => Some(form.values.name)}
              loading={operationRequest->ApiRequest.isLoading ||
                multisigRequest->ApiRequest.isLoading}
              onOperationAndSigningIntent={handleOperationAndSigningIntent(account.address)}
            />
          | SubmittedStep(originator, hash) =>
            let onPressCancel = _ => {
              closeAction()
              setDefaultAccount(originator)
              Routes.push(Operations)
            }
            <SubmittedView hash onPressCancel submitText=I18n.Btn.go_operations />
          }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>
}
