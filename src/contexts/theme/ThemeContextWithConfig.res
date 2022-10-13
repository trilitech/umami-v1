let useConfigFileState = () => {
  let {write, configFile} = ConfigFileContext.useConfigFile()

  let theme = configFile.theme->Option.getWithDefault(#system)
  let setTheme = f => write(c => {...c, theme: Some(f)})

  (theme, setTheme)
}

@react.component
let make = (~children) => {
  let (theme, setTheme) = useConfigFileState()

  <ThemeContextBase theme setTheme> children </ThemeContextBase>
}
