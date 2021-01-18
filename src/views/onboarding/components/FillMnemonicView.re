open ReactNative;

module StateLenses = [%lenses type state = {words: array(string)}];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

module InputWord = {
  [@react.component]
  let make = (~arrayUpdateByIndex, ~getNestedFieldError, ~index, ~word) => {
    let handleChange =
      React.useMemo1(
        () => {arrayUpdateByIndex(~field=StateLenses.Words, ~index)},
        [|index|],
      );

    let error =
      React.useMemo1(
        () => {
          getNestedFieldError(
            VerifyMnemonicForm.ReSchema.Field(StateLenses.Words),
            index,
          )
        },
        [|index|],
      );

    <InputMnemonicWord displayIndex=index value=word handleChange error />;
  };
};

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
      ~initialState={words: mnemonic},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <>
    <View style=Style.(array([|styles##wordsList, style(~zIndex=2, ())|]))>
      {form.state.values.words
       ->Array.mapWithIndex((index, word) =>
           <React.Fragment key={index->string_of_int}>
             <View
               style=Style.(
                 array([|
                   styles##wordItem,
                   style(
                     ~zIndex=form.state.values.words->Array.size - index,
                     (),
                   ),
                 |])
               )>
               <InputWord
                 index
                 word
                 arrayUpdateByIndex={form.arrayUpdateByIndex}
                 getNestedFieldError={form.getNestedFieldError}
               />
             </View>
             {index mod 2 == 0 ? <View style=styles##wordSpacer /> : React.null}
           </React.Fragment>
         )
       ->React.array}
    </View>
    <View
      style=Style.(
        array([|FormStyles.formActionSpaceBetween, style(~zIndex=1, ())|])
      )>
      <Buttons.Form text=I18n.btn#back onPress=onPressCancel />
      <Buttons.SubmitPrimary text=I18n.btn#continue onPress=onSubmit />
    </View>
  </>;
};
