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

module StateLenses = %lenses(type state = {read: bool})
module DisclaimerForm = ReForm.Make(StateLenses)

let styles = {
  open Style
  StyleSheet.create({
    "modal": style()->unsafeAddStyle({"boxShadow": "none"}),
    "title": style(~marginBottom=30.->dp, ~textAlign=#center, ()),
  })
}

@react.component
let make = (~onSign) => {
  let theme = ThemeContext.useTheme()
  let (firstLinkHovered, setFirstLinkHovered) = React.useState(_ => false)
  let (secondLinkHovered, setSecondLinkHovered) = React.useState(_ => false)

  let onAgree = () => Disclaimer.sign()

  let form: DisclaimerForm.api = DisclaimerForm.use(
    ~validationStrategy=OnDemand,
    ~schema={
      open DisclaimerForm.Validation
      Schema(true_(Read))
    },
    ~onSubmit=_ => {
      onAgree()
      onSign(false)
      None
    },
    ~initialState={read: false},
    ~i18n=FormUtils.i18n,
    (),
  )

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  <ModalFormView
    style={
      open Style
      array([styles["modal"], style(~backgroundColor=theme.colors.barBackground, ())])
    }>
    <Typography.Headline style={styles["title"]}>
      {React.string("Accept and continue")}
    </Typography.Headline>
    <View>
        <CheckboxItem
          labelFontWeightStyle=#regular
          labelStyle={Style.style(~color=Typography.getColor(#highEmphasis, theme), ())}
          value=form.values.read
          handleChange={form.handleChange(Read)}
        />
        <div style={
          ReactDOM.Style.make(
            ~fontFamily="SourceSansPro",
            ~display="block",
            ~position="absolute",
            ~marginLeft="40px",
            ~color=Typography.getColor(#highEmphasis, theme),
            ()
          )}>
          <p style={ReactDOM.Style.make(~display="inline", ())}>{React.string("I confirm that I have read and agreed with the ")}</p>
          <a
            onMouseEnter={_ => setFirstLinkHovered(_ => true)}
            onMouseLeave={_ => setFirstLinkHovered(_ => false)}
            href="https://umamiwallet.com/tos.html" style={
              ReactDOM.Style.make(
                ~display="inline",
                ~color=firstLinkHovered ? theme.colors.primaryButtonBackground : "inherit",
                ~fontFamily="SourceSansPro",
                ())
              } target="_blank">
            {React.string("Terms of Use")}
          </a>
          <p style={ReactDOM.Style.make(~display="inline", ())}>{React.string(" and the ")}</p>
          <a
            onMouseEnter={_ => setSecondLinkHovered(_ => true)}
            onMouseLeave={_ => setSecondLinkHovered(_ => false)}
            href="https://umamiwallet.com/privacypolicy.html" style={
              ReactDOM.Style.make(
                ~display="inline",
                ~fontFamily="SourceSansPro",
                ~color=secondLinkHovered ? theme.colors.primaryButtonBackground : "inherit",
                ())
              } target="_blank">
            {React.string("Privacy Policy")}
          </a>
        </div>
      <View>

      </View>
      <Buttons.SubmitPrimary
        text=I18n.Btn.disclaimer_agree
        onPress={_ => form.submit()}
        disabledLook={!formFieldsAreValids}
        style={
          open Style
          style(~marginTop=40.->dp, ())
        }
      />
    </View>
  </ModalFormView>
}
