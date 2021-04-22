open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "wordsList": style(~flexDirection=`row, ~flexWrap=`wrap, ()),
      "wordItem":
        style(
          ~marginVertical=2.->dp,
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=40.->pct,
          (),
        ),
      "wordSpacer": style(~width=20.->dp, ()),
    })
  );

module FormatSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~marginLeft=16.->dp,
            ~flexDirection=`row,
            ~flexWrap=`wrap,
            (),
          ),
        "label": style(~marginBottom=4.->dp, ()),
        "selector": style(~marginBottom=24.->dp, ()),

        "button": style(~flex=1., ~paddingVertical=11.->dp, ()),
      })
    );

  type format =
    | Words24
    | Words15
    | Words12;
  let toInt =
    fun
    | Words24 => 24
    | Words15 => 15
    | Words12 => 12;

  let toString = ft => I18n.t#words(ft->toInt);

  let render = ft =>
    <Typography.Body1 style=styles##item>
      {ft->toString->React.string}
    </Typography.Body1>;
  let renderButton = (ft, _hasError) =>
    <View style=styles##button>
      {switch (ft) {
       | Some(ft) => render(ft)
       | None => render(Words24)
       }}
    </View>;

  let onValueChange = (setMnemonics, mnemonics, format) => {
    let vInt = format->toInt;

    let len = mnemonics->Array.length;

    let mnemonics =
      if (len > vInt) {
        mnemonics->Array.slice(~offset=0, ~len=vInt);
      } else if (len < vInt) {
        mnemonics->Array.concat(Array.make(vInt - len, ""));
      } else {
        mnemonics;
      };

    setMnemonics(format, mnemonics);
  };

  [@react.component]
  let make = (~value, ~onValueChange) => {
    <>
      <FormLabel style=styles##label label=I18n.label#recovery_phrase_format />
      <Selector
        style=styles##selector
        items=[|Words24, Words15, Words12|]
        getItemKey={ft => ft->toInt->Int.toString}
        renderItem=render
        selectedValueKey={value->toInt->Int.toString}
        onValueChange
        renderButton
        keyPopover="MnemonicsFormatSelector"
      />
    </>;
  };
};

module StateLenses = [%lenses
  type state = {
    format: FormatSelector.format,
    words: array(string),
  }
];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

let stateField = StateLenses.Words;
let formField = VerifyMnemonicForm.ReSchema.Field(stateField);

[@react.component]
let make = (~mnemonic, ~setMnemonic, ~onPressCancel, ~goNextStep) => {
  let form: VerifyMnemonicForm.api =
    VerifyMnemonicForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        VerifyMnemonicForm.Validation.(
          Schema(
            custom(
              ({words}) => {
                let errors =
                  words
                  ->Array.mapWithIndex((index, word) => {
                      Js.String.length(word) == 0
                        ? Some({
                            ReSchema.error: "Invalid word",
                            index,
                            name: "word",
                          })
                        : None
                    })
                  ->Array.keepMap(e => e);

                let fieldState: ReSchema.fieldState =
                  errors->Array.size == 0 ? Valid : NestedErrors(errors);

                fieldState;
              },
              Words,
            ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          setMnemonic(_ => state.values.words);
          goNextStep();
          None;
        },
      ~initialState={format: Words24, words: mnemonic},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <FormatSelector
      value={form.values.format}
      onValueChange={format =>
        FormatSelector.onValueChange(
          (fmt, words) => {
            form.handleChange(Words, words);
            form.handleChange(Format, fmt);
          },
          form.values.words,
          format,
        )
      }
    />
    <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
      <View style=styles##wordsList>
        {form.state.values.words
         ->Array.mapWithIndex((index, word) =>
             <React.Fragment key={index->string_of_int}>
               <View style=styles##wordItem>
                 <InputMnemonicWord
                   index
                   word
                   arrayUpdateByIndex={form.arrayUpdateByIndex}
                   getNestedFieldError={form.getNestedFieldError}
                   formField
                   stateField
                 />
               </View>
               {index mod 2 == 0
                  ? <View style=styles##wordSpacer /> : React.null}
             </React.Fragment>
           )
         ->React.array}
      </View>
    </DocumentContext.ScrollView>
    <View style=FormStyles.formActionSpaceBetween>
      <Buttons.Form text=I18n.btn#back onPress=onPressCancel />
      <Buttons.SubmitPrimary
        text=I18n.btn#continue
        onPress=onSubmit
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
