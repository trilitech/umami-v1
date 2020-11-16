open ReactNative;

module StateLenses = [%lenses type state = {words: array(string)}];

module VerifyMnemonicForm = ReForm.Make(StateLenses);

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
      "formAction":
        style(
          ~marginTop=28.->dp,
          ~flexDirection=`row,
          ~justifyContent=`center,
          (),
        ),
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
                  ->Belt.Array.mapWithIndex((index, word) => {
                      Js.String.length(word) == 0
                        ? Some({
                            ReSchema.error: "Invalid word",
                            index,
                            name: "word",
                          })
                        : None
                    })
                  ->Belt.Array.keepMap(e => e);

                let fieldState: ReSchema.fieldState =
                  errors->Belt.Array.size == 0 ? Valid : NestedErrors(errors);

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
       ->Belt.Array.mapWithIndex((index, word) =>
           <React.Fragment key={index->string_of_int}>
             <View
               style=Style.(
                 array([|
                   styles##wordItem,
                   style(
                     ~zIndex=form.state.values.words->Belt.Array.size - index,
                     (),
                   ),
                 |])
               )>
               <InputMnemonicWord
                 displayIndex=index
                 value=word
                 handleChange={form.arrayUpdateByIndex(~field=Words, ~index)}
                 error={form.getNestedFieldError(Field(Words), index)}
               />
             </View>
             {index mod 2 == 0 ? <View style=styles##wordSpacer /> : React.null}
           </React.Fragment>
         )
       ->React.array}
    </View>
    <View style=Style.(array([|styles##formAction, style(~zIndex=1, ())|]))>
      <FormButton text="CANCEL" onPress=onPressCancel />
      <FormButton text="CONTINUE" onPress=onSubmit />
    </View>
  </>;
};
