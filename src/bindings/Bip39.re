[@bs.module "bip39"] external generate: int => string = "generateMnemonic";

[@bs.module "bip39"] [@bs.scope "wordlists"]
external wordlistsEnglish: array(string) = "english";
