import sys
import argparse
import os
import json
import datetime
import requests

parser = argparse.ArgumentParser(description='Generates a Blog Review report')
parser.add_argument('-t', '--token', help='Gitlab API token')
args = vars(parser.parse_args())

def days_between(d1, d2):
    d1 = datetime.datetime.strptime(d1, "%Y-%m-%d")
    d2 = datetime.datetime.strptime(d2, "%Y-%m-%d")
    return abs((d2 - d1).days)

def days_add(d1, dur):
    d1 = datetime.datetime.strptime(d1, "%Y-%m-%d") + datetime.timedelta(days=dur)
    return d1.strftime("%Y-%m-%d")

issues = []
includedtag = 'Gantt'
defaultduration = 30

projectid = "19119758"
issuesurl = "https://gitlab.com/api/v4/issues?project_id={0}".format(projectid)
response = requests.get(issuesurl, headers={"PRIVATE-TOKEN": args['token']})
issues = json.loads(response.content)
repourl = "https://gitlab.com/api/v4/projects/{0}".format(projectid)
response = requests.get(repourl, headers={"PRIVATE-TOKEN": args['token']})
repo = json.loads(response.content)
#print([x['state'] for x in issues])
#for issue in issues:
#  print("\n".join(list(issue.keys())))

milestones = set()
for issue in issues:
  if issue['milestone'] is not None and 'title' in issue['milestone'].keys():
    milestones.add(issue['milestone']['title'])
milestones = list(milestones)
milestones.append(None)

print("```mermaid")
print("gantt")
print("    title {0}".format(repo['name']))

for milestone in milestones:
  issuelist = []
  if milestone is not None:
    print("    section {0}".format(milestone))
    issuelist = list(filter(lambda x: 'milestone' in x.keys() and x['milestone'] is not None and x['milestone']['title'] == milestone, issues))
  else:
    print("    section Other")
    issuelist = list(filter(lambda x: 'milestone' not in x.keys() or x['milestone'] is None, issues))
  for issue in issuelist:
    #print(issue)
    if 'labels' in issue.keys():
      if includedtag not in issue['labels']:
        continue

    start_date = None
    due_date = None
    duration = None

    milestone_sd = None
    created_at = None
    if 'milestone' in issue.keys() and issue['milestone'] is not None and 'start_date' in issue['milestone'].keys():
      milestone_sd = issue['milestone']['start_date']
    if 'created_at' in issue.keys() and issue['created_at'] is not None:
      created_at = issue['created_at'][0:10]
    if milestone_sd is not None and created_at is not None:
      if milestone_sd > created_at:
        start_date = milestone_sd
      else:
        start_date = created_at
    elif milestone_sd is not None:
      start_date = milestone_sd
    elif created_at is not None:
      start_date = created_at

    if 'due_date' in issue.keys():
      due_date = issue['due_date']
    elif 'closed_at' in issue.keys() and issue['closed_at'] is not None:
      due_date = issue['closed_at'][0:10]
    elif 'milestone' in issue.keys() and issue['milestone'] is not None and 'due_date' in issue['milestone'].keys():
      due_date = issue['milestone']['due_date']

    if due_date is None and start_date is not None:
      due_date = days_add(start_date, defaultduration)
    print("    {0}{1}:active, {2}, {3}".format(issue['title'], " "*(40 - len(issue['title'])), start_date, due_date))

print("```")

