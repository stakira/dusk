
{
  'target_defaults': {
    'msvs_configuration_attributes': {
      # DON'T ADD ANYTHING NEW TO THIS BLOCK UNLESS YOU REALLY REALLY NEED IT!
      # This block adds *project-wide* configuration settings to each project
      # file.  It's almost always wrong to put things here. Instead, specify
      # your custom msvs_configuration_attributes in a target_defaults section
      # of a .gypi file that is explicitly included.
      'OutputDirectory': '<(DEPTH)\\build\\$(ConfigurationName)',
      'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)',
    },
  }
}
