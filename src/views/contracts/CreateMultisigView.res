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
    "body": style(),
    "description": style(~marginBottom=16.->dp, ()),
    "textInput": style(~maxWidth=415.->dp, ()),
    "actions": style(~flexDirection=#row, ()),
  })
}

module StepView = {
  let styles = {
    open Style
    StyleSheet.create({
      "body": style(
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
        {step->Int.toString->React.string}
      </View>
    }
  }

  @react.component
  let make = (~step: int, ~title, ~folded, ~children) => {
    let theme = ThemeContext.useTheme()

    <View
      style={
        open Style
        array([styles["body"], style(~backgroundColor=theme.colors.elevatedBackground, ())])
      }>
      <Badge step disabled=folded />
      <View style={styles["content"]}>
        <Typography.Subtitle2
          colorStyle={folded ? #disabled : #highEmphasis} style={styles["title"]}>
          {title->React.string}
        </Typography.Subtitle2>
        {ReactUtils.onlyWhen(children, !folded)}
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

    <StepView step=1 title=I18n.Title.name_contract folded={currentStep != 1}>
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.name_multisig->React.string}
      </Typography.Body1>
      <FormGroupTextInput
        style={styles["textInput"]}
        label=I18n.Label.add_contract_name
        value=form.values.name
        handleChange=form.handleChange(Name)
        placeholder=I18n.Input_placeholder.add_contract_name
        error={list{
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Name)),
        }->Option.firstSome}
      />
      <View style={styles["actions"]}>
        <Buttons.SubmitPrimary
          text=I18n.Btn.continue
          onPress={_ => Js.log("yo")}//form.submit()}
          style=FormStyles.formSubmit
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </StepView>
  }
}

module Step2 = {
  module Form = {
    module StateLenses = %lenses(type state = {owners: array<string>, threshold: int})

    include ReForm.Make(StateLenses)
  }

  @react.component
  let make = (~currentStep, ~back, ~action) => {
    let form = Form.use(
      ~schema={
        open Form.Validation
        Schema(custom(state => Valid, Owners))
      },
      ~onSubmit=({state, _}) => {
        action(state.values.owners, state.values.threshold)
        None
      },
      ~initialState={owners: [], threshold: 1},
      ~i18n=FormUtils.i18n,
      (),
    )

    <StepView step=2 title=I18n.Title.set_owners_and_threshold folded={currentStep != 2}>
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.set_owners->React.string}
      </Typography.Body1>
      <FormGroupTextInput
        style={styles["textInput"]}
        label=I18n.Label.owners
        value=""
        handleChange={_ => ()}
        placeholder=I18n.Input_placeholder.set_owner
        error={None}
      />
      <ButtonAction
        style={
          open Style
          style(~alignSelf=#flexStart, ~marginTop=-15.->dp, ~marginBottom=28.->dp, ())
        }
        onPress={_ => ()}
        text=I18n.Btn.add_another_owner
        icon=Icons.Add.build
        primary=true
      />
      <Typography.Body1 style={styles["description"]}>
        {I18n.Expl.set_threshold->React.string}
      </Typography.Body1>
      <View style={styles["actions"]}>
        <Buttons.SubmitSecondary
          text=I18n.Btn.back
          onPress={_ => back()}
          style={
            open Style
            StyleSheet.flatten([FormStyles.formSubmit, style(~marginRight=21.->dp, ())])
          }
        />
        <Buttons.SubmitPrimary
          text=I18n.Btn.continue onPress={_ => ()} style=FormStyles.formSubmit disabledLook={true}
        />
      </View>
    </StepView>
  }
}

@react.component
let make = (~closeAction) => {
  let (currentStep, setCurrentStep) = React.useState(_ => 1)

  let closeButton =
    <ModalFormView.CloseButton
      closing={ModalFormView.confirm(~actionText=I18n.Btn.cancel, closeAction)}
    />

  <View style={styles["body"]}>
    <Page.Header right=closeButton>
      <Typography.Headline style=Styles.title>
        {I18n.Title.create_new_multisig->React.string}
      </Typography.Headline>
      <Step1
        currentStep
        action={name => {
          Js.log(name)
          setCurrentStep(_ => 2)
        }}
      />
      <Step2 currentStep back={_ => setCurrentStep(_ => 1)} action={(_, _) => ()} />
      <StepView step=3 title=I18n.Title.review_and_submit folded={currentStep != 3}>
        {<> </>}
      </StepView>
    </Page.Header>
  </View>
}
