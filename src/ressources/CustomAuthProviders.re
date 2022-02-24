let getComponent =
  fun
  | `google => <SVGGoogle />
  | `facebook => <SVGFacebook />
  | `discord => <SVGDiscord />
  | `reddit => <SVGReddit />
  | `github => <SVGGithub />
  | `twitter => <SVGTwitter />
  | `twitch => <SVGTwitch />;

let getIcon =
  fun
  | `google => Icons.Google.build
  | `facebook => Icons.Facebook.build
  | `discord => Icons.Discord.build
  | `reddit => Icons.Reddit.build
  | `github => Icons.Github.build
  | `twitter => Icons.Twitter.build
  | `twitch => Icons.Twitch.build;

let getHandleKindI18n =
  fun
  | `google => I18n.email
  | `facebook => I18n.username
  | `discord => I18n.username
  | `reddit => I18n.username
  | `github => I18n.username
  | `twitter => I18n.username
  | `twitch => I18n.username;
