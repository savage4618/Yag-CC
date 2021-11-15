{{$join := ############# }} {{/* <-- replace ##### with Arena join channel ID */}}
{{$arena := ############# }} {{/* <-- replace ##### with Arena channel ID */}}
{{$mod :=  &&&&&&&&&&&& }} {{/* <-- replace &&&&& with mode role ID */}}
{{$arenaRole := &&&&&&&&&&&&&& }} {{/* <-- replace &&&&& with role ID that unlocks #arena channel */}}

{{$expiryTime := 15}} {{/* time in minutes to reset arena list after being inactive */}}
{{$success := "✅"}}
{{$error := "❌"}}
{{$cookie := "🍪"}}

{{/* ----- don't edit if u don't know what you doing -------*/}}
{{if not .ExecData}}
	{{$list := ""}}{{$count := 1}}{{$msg := ""}}
	{{$alist := cslice.AppendSlice (or (dbGet .Channel.ID "alist").Value cslice)}}
	{{$ex := or (and (reFind "a_" .Guild.Icon) "gif" ) "png" }}
	{{$icon := print "https://cdn.discordapp.com/icons/" .Guild.ID "/" .Guild.Icon "." $ex "?size=1024" }}
	{{$embed := sdict "author" (sdict "name" (print .Guild.Name) "icon_url" $icon) "timestamp" currentTime "footer" (sdict "text" "\"a help\" for help")}}

	{{if and .CmdArgs (eq $join .Channel.ID)}}
		{{$cmd := index .CmdArgs 0 | lower}}
		{{if and (eq $cmd "join") (le (len $alist) 10)}}
			{{if not (in $alist .User.ID)}}
				{{$alist = $alist.Append .User.ID}}
				{{dbSet .Channel.ID "alist" $alist}}
				{{range $alist}}
					{{- $list = printf "%s`%02d. %d` | <@%d>\n" $list $count . .}}{{$count = add 1 $count -}}
				{{end}}
				{{$embed.Set "description" $list}}{{$embed.Set "title" (print "Arena List " (len $alist) "/10")}}
				{{if $smsg := dbGet .Channel.ID "smsg"}}{{deleteMessage nil $smsg.Value 1}}{{end}}
				{{$msg = sendMessageRetID nil (cembed $embed)}}
				{{dbSet .Channel.ID "smsg" (str $msg)}}
				{{addReactions $success}}
				{{if eq (len $alist) 2}}
					{{deleteMessage nil $msg 1}}
					{{sendMessage nil (complexMessage "content" (print "List sent to <#" $arena ">.") "embed" (cembed "title" "Boosted Arena" "timestamp" currentTime "thumbnail" (sdict "url" $icon) "description" $list))}}
					{{dbDel .Channel.ID "alist"}}
					{{dbDel .Channel.ID "smsg"}}
					{{$list1 := ""}}
					{{range $alist}}
						{{- giveRoleID . $arenaRole -}}
						{{- takeRoleID . $arenaRole 180 -}}
						{{- $list1 = printf "%s<@%d> " $list1 . -}}
					{{end}}
					{{addMessageReactions $arena (sendMessageRetID $arena (printf "%s\n\n```\nRpg arena %s```" $list1 $list1)) $cookie}}
				{{end}}
			{{else}}
				{{deleteMessage nil (sendMessageRetID nil "you are already in list") 3}}
				{{addReactions $error}}
			{{end}}
		{{scheduleUniqueCC .CCID nil (mult $expiryTime 60) "alist" (sdict "msg" (str $msg))}}
		{{else if eq $cmd "leave"}}
			{{if (in $alist .User.ID)}}
				{{$new := cslice}}
				{{range $alist}}
					{{if not (eq (toInt .) (toInt $.User.ID))}}
						{{$new = $new.Append (toInt .)}}
					{{end}}
				{{end}}
				{{range $new}}
					{{- $list = printf "%s`%02d. %d` | <@%d>\n" $list $count . .}}{{$count = add 1 $count -}}
				{{end}}
				{{$embed.Set "description" $list}}{{$embed.Set "title" (print "Arena List " (len $new) "/10")}}
				{{if $smsg := dbGet .Channel.ID "smsg"}}{{deleteMessage nil $smsg.Value 1}}{{end}}
				{{$msg = sendMessageRetID nil (cembed $embed)}}
				{{dbSet .Channel.ID "smsg" (str $msg)}}
				{{dbSet .Channel.ID "alist" $new}}
				{{addReactions $success}}
			{{else}}
				{{deleteMessage nil (sendMessageRetID nil "you are not in list") 3}}
			{{end}}
		{{else if eq $cmd "list"}}
			{{range $alist}}
				{{- $list = printf "%s`%02d. %d` | <@%d>\n" $list $count . .}}{{$count = add 1 $count -}}
			{{end}}
			{{$embed.Set "description" $list}}{{$embed.Set "title" (print "Arena List " (len $alist) "/10")}}
			{{if $lmsg := dbGet .Channel.ID "lmsg"}}{{deleteMessage nil $lmsg.Value 1}}{{end}}
			{{$lmsg := sendMessageRetID nil (cembed $embed)}}
			{{dbSet .Channel.ID "lmsg" (str $lmsg)}}
		{{else if eq $cmd "reset"}}
			{{if and (hasRoleID $mod) $mod}}
				{{dbDel .Channel.ID "alist"}}
				{{addReactions $success}}
			{{else}}
				{{deleteMessage nil (sendMessage nil "You don't have permissions to reset list!") 5}}
				{{addReactions $error}}
			{{end}}
		{{else if eq $cmd "help"}}
			{{$embed.Set "title" "Arena Help" }}
			{{$embed.Set "description" (print "1.  `a join` : To join the list.\n2. `a leave` : To leave the list.\n3. `a reset`: To reset the list. (Need Mod role!).\n4. `a list` : To view the list")}}
			{{deleteMessage nil (sendMessageRetID nil (complexMessage "content" .User.Mention "embed" (cembed $embed))) 60}}
		{{end}}
		{{deleteTrigger 3}}
	{{end}}
{{else}}
	{{deleteMessage nil .ExecData.msg 1}}
	{{dbDel .Channel.ID "alist"}}
󠂪󠂪󠂪󠂪	{{dbDel .Channel.ID "smsg"}}
	{{dbDel .Channel.ID "lmsg"}}
	{{sendMessage nil (print "Arena got reset due to inactivity!\nNo player joined the list for past " $expiryTime " minutes.")}}
{{end}}