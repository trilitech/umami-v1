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
    "description": style(~marginBottom=20.->dp, ()),
    "textInput": style(~maxWidth=415.->dp, ()),
    "addressInput": style(
      ~flex=1.,
      ~maxWidth=415.->dp,
      ~marginBottom=16.->dp,
      ~alignContent=#center,
      (),
    ),
    "numberInput": style(~flex=1., ~maxWidth=201.->dp, ()),
    "suffix": style(~marginLeft=8.->dp, ()),
    "row": style(~flexDirection=#row, ~alignContent=#center, ~alignItems=#center, ()),
    "selectorItemContainer": style(
      ~height=34.->dp,
      ~marginHorizontal=16.->dp,
      ~flexDirection=#row,
      ~alignItems=#center,
      (),
    ),
    "selectorButtonContent": style(
      ~width=201.->dp,
      ~height=44.->dp,
      ~display=#flex,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~justifyContent=#spaceBetween,
      (),
    ),
    "info": style(~maxWidth=415.->dp, ~marginVertical=4.->dp, ()),
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

module Step1 = {
  module Form = {
    module StateLenses = %lenses(type state = {name: string})

    include ReForm.Make(StateLenses)
  }

  @react.component
  let make = (~currentStep, ~action) => {
    let form = Form.use(
      ~schema={
        open Form.Validation
        Schema(nonEmpty(Name))
      },
      ~onSubmit=({state}) => {
        action(state.values.name)
        None
      },
      ~initialState={name: ""},
      ~i18n=FormUtils.i18n,
      (),
    )

    let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

    <StepView step=1 currentStep title=I18n.Title.name_contract>
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.name_multisig->React.string}
      </Typography.Body1>
      <FormGroupTextInput
        style={styles["textInput"]}
        label=I18n.Label.add_contract_name
        value=form.values.name
        handleChange={form.handleChange(Name)}
        placeholder=I18n.Input_placeholder.add_contract_name
        error={list{
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Name)),
        }->Option.firstSome}
        onSubmitEditing={_ => form.submit()}
      />
      <View style={styles["row"]}>
        <Buttons.SubmitPrimary
          text=I18n.Btn.continue
          onPress={_ => form.submit()}
          style=FormStyles.formSubmit
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </StepView>
  }
}

module Step2 = {
  module Form = {
    module StateLenses = %lenses(
      type state = {
        owners: array<FormUtils.Alias.any>,
        threshold: int,
      }
    )

    include ReForm.Make(StateLenses)
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

  let renderItem = item =>
    <View style={styles["selectorItemContainer"]}>
      <Typography.ButtonSecondary fontSize=14.>
        {item->Int.toString->React.string}
      </Typography.ButtonSecondary>
    </View>

  @react.component
  let make = (~currentStep, ~back, ~action) => {
    let theme = ThemeContext.useTheme()

    let form = Form.use(
      ~schema={
        open Form.Validation
        Schema(
          custom(values => validateOwners(values.owners), Owners) + custom(_ => Valid, Threshold),
        )
      },
      ~onSubmit=({send, state}) => {
        action(state.values.owners, state.values.threshold)
        None
      },
      ~initialState={owners: [FormUtils.Alias.AnyString("")], threshold: 1},
      ~i18n=FormUtils.i18n,
      (),
    )

    let aliasesRequest = StoreContext.Aliases.useRequest()
    let aliases =
      aliasesRequest->ApiRequest.getDoneOk->Option.getWithDefault(PublicKeyHash.Map.empty)

    let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

    <StepView step=2 currentStep title=I18n.Title.set_owners_and_threshold>
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.set_multisig_owners->React.string}
      </Typography.Body1>
      {form.values.owners
      ->Array.mapWithIndex((i, value) => {
        let key = i->Int.toString
        <View key style={styles["row"]}>
          <View style={styles["addressInput"]}>
            <FormGroupContactSelector
              keyPopover={"formGroupContactSelector" ++ key}
              label={i == 0 ? I18n.Label.owners : ""}
              filterOut={None}
              aliases
              value
              handleChange={newOwner =>
                form.handleChangeWithCallback(Owners, owners =>
                  owners->Array.mapWithIndex((j, owner) => i == j ? newOwner : owner)
                )}
              error={form.getNestedFieldError(Field(Owners), i)}
            />
          </View>
          {<IconButton
            onPress={_ => {
              if form.values.threshold == form.values.owners->Array.length {
                form.handleChangeWithCallback(Threshold, threshold => threshold - 1)
              }
              form.handleChangeWithCallback(Owners, owners =>
                owners->Array.keepWithIndex((_, j) => i != j)
              )
            }}
            icon=Icons.Delete.build
            size=40.
            style={
              open Style
              style(~marginLeft=8.->dp, ~marginBottom=22.->dp, ())
            }
          />->ReactUtils.onlyWhen(i != 0)}
        </View>
      })
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
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.set_multisig_threshold->React.string}
      </Typography.Body1>
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
          items={Array.range(1, form.values.owners->Array.length)}
          getItemKey={item => item->Int.toString}
          renderItem
          selectedValueKey={form.values.threshold->Int.toString}
          onValueChange={item => form.handleChange(Threshold, item)}
          renderButton={(selectedItem, _) => {
            selectedItem->Option.mapWithDefault(React.null, item => renderItem(item))
          }}
          keyPopover="Selector"
        />
        <Typography.Body1 style={styles["suffix"]}>
          {I18n.Expl.out_of(form.values.owners->Array.length->Int.toString)->React.string}
        </Typography.Body1>
      </View>
      <View style={styles["row"]}>
        <Buttons.SubmitSecondary
          text=I18n.Btn.back
          onPress={_ => back()}
          style={
            open Style
            StyleSheet.flatten([FormStyles.formSubmit, style(~marginRight=21.->dp, ())])
          }
        />
        <Buttons.SubmitPrimary
          text=I18n.Btn.continue
          onPress={_ => form.submit()}
          style=FormStyles.formSubmit
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </StepView>
  }
}

module Step3 = {
  module Info = {
    @react.component
    let make = (~children: React.element) => {
      let theme = ThemeContext.useTheme()

      <View
        style={
          open Style
          style(
            ~borderRadius=4.,
            ~maxWidth=415.->dp,
            ~minHeight=44.->dp,
            ~backgroundColor=theme.colors.stateDisabled,
            ~marginTop=4.->dp,
            ~marginBottom=8.->dp,
            ~paddingHorizontal=16.->dp,
            ~paddingVertical=12.->dp,
            (),
          )
        }>
        children
      </View>
    }
  }
  @react.component
  let make = (~currentStep, ~settings, ~back, ~action) => {
    <StepView step=3 currentStep title=I18n.Title.review_and_submit>
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.review_multisig->React.string}
      </Typography.Body1>
      <Typography.Subtitle1> {I18n.Title.contract_name->React.string} </Typography.Subtitle1>
      <Info> <Typography.Body1> {settings.Multisig.name->React.string} </Typography.Body1> </Info>
      <Typography.Subtitle1
        style={
          open Style
          style(~marginTop=16.->dp, ())
        }>
        {I18n.Title.owners(settings.owners->Array.length->Int.toString)->React.string}
      </Typography.Subtitle1>
      {settings.owners
      ->Array.mapWithIndex((i, owner) =>
        switch owner {
        | Address(address) =>
          <OperationSummaryView.EntityInfo
            key={i->Int.toString} style={styles["info"]} address={Some(address)}
          />
        | Alias(alias) => <OperationSummaryView.EntityInfo address={Some(alias.address)} />
        }
      )
      ->React.array}
      <Typography.Subtitle1
        style={
          open Style
          style(~marginTop=16.->dp, ())
        }>
        {I18n.Title.approval_threshold->React.string}
      </Typography.Subtitle1>
      <Info>
        <Typography.Body1>
          {I18n.Label.out_of(
            settings.threshold->Int.toString,
            settings.owners->Array.length->Int.toString,
          )->React.string}
        </Typography.Body1>
      </Info>
      <View style={styles["row"]}>
        <Buttons.SubmitSecondary
          text=I18n.Btn.back
          onPress={_ => back()}
          style={
            open Style
            StyleSheet.flatten([FormStyles.formSubmit, style(~marginRight=21.->dp, ())])
          }
        />
        <Buttons.SubmitPrimary
          text=I18n.Btn.submit onPress={_ => action()} style=FormStyles.formSubmit
        />
      </View>
    </StepView>
  }
}

@react.component
let make = (~closeAction) => {
  let (currentStep, setCurrentStep) = React.useState(_ => 1)
  let (settings, setSettings) = React.useState(_ => {Multisig.name: "", owners: [], threshold: 1})

  let closeButton =
    <ModalFormView.CloseButton
      closing={ModalFormView.confirm(~actionText=I18n.Btn.cancel, closeAction)}
    />

  <>
    <Page.Header right=closeButton>
      <Typography.Headline style=Styles.title>
        {I18n.Title.create_new_multisig->React.string}
      </Typography.Headline>
      <Step1
        currentStep
        action={name => {
          setSettings(_ => {...settings, name: name})
          setCurrentStep(_ => 2)
        }}
      />
      <Step2
        currentStep
        back={_ => setCurrentStep(_ => 1)}
        action={(owners, threshold) => {
          setSettings(_ => {
            ...settings,
            owners: owners->Array.keepMap(owner =>
              switch owner {
              | FormUtils.Alias.Valid(alias) => Some(alias)
              | _ => None
              }
            ),
            threshold: threshold,
          })
          setCurrentStep(_ => 3)
        }}
      />
      <Step3 currentStep settings back={_ => setCurrentStep(_ => 2)} action={_ => ()} />
    </Page.Header>
  </>
}
